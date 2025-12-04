# orchestrator/app.py
from fastapi import FastAPI, HTTPException
from pydantic import BaseModel
import subprocess
import shlex
import os

app = FastAPI()

# try env var
LOGFILE = os.environ.get("DEVICE_LOGFILE", None)
if LOGFILE is None:
    # common fallbacks relative to repo root
    cand = [
        os.path.join(os.path.dirname(os.path.dirname(__file__)), "device_agent", "device_agent.log"),
        os.path.join(os.path.dirname(os.path.dirname(__file__)), "device_agent", "device_agent.log"), # same but keep
        "/var/log/device_agent.log",
        "./device_agent/device_agent.log",
    ]
    LOGFILE = next((p for p in cand if os.path.isfile(p)), cand[0])



SCRIPTS_DIR = os.path.join(os.path.dirname(__file__), "..", "infra", "fault_injection")

class Action(BaseModel):
    action_type: str  # add_delay | remove_delay
    params: dict = {}

@app.post("/apply_action")
def apply_action(a: Action):
    if a.action_type == "add_delay":
        script = os.path.join(SCRIPTS_DIR, "add_delay.sh")
    elif a.action_type == "remove_delay":
        script = os.path.join(SCRIPTS_DIR, "remove_delay.sh")
    else:
        raise HTTPException(status_code=400, detail="unknown action_type")

    # Ensure script exists
    if not os.path.isfile(script):
        raise HTTPException(status_code=500, detail=f"script missing: {script}")

    # Run script (may require sudo) - use bash to run
    try:
        # Note: this will run synchronously and return stdout/stderr
        res = subprocess.run(["bash", script], capture_output=True, text=True, timeout=10)
        return {"rc": res.returncode, "stdout": res.stdout.strip(), "stderr": res.stderr.strip()}
    except subprocess.TimeoutExpired:
        raise HTTPException(status_code=500, detail="script timeout")
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.get("/telemetry")
def get_last_telemetry():
    if not os.path.isfile(LOGFILE):
        raise HTTPException(status_code=404, detail="logfile not found")
    try:
        # read last non-empty line
        with open(LOGFILE, "r") as f:
            lines = [ln.strip() for ln in f.readlines() if ln.strip()]
            if not lines:
                raise HTTPException(status_code=404, detail="no telemetry yet")
            return {"last": lines[-1]}
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))
