# Self-Healing IoT Framework 🛡️⚡  
**Autonomous fault detection + instant self-healing for edge devices**

Real-time C++ agent on device + Python orchestrator + MQTT communication + fault injection for testing.

## Current Features (Live & Working)
- C++ agent runs on IoT device, sends heartbeat + metrics every 5s
- Python orchestrator (`app.py`) monitors health via MQTT
- Detects network latency, process crash, or missed heartbeats
- Triggers automatic healing (restart agent, remove delay, etc.)
- Fault injection scripts to test the self-healing logic

## Project Structure
self_healing_iot/
├── device_agent/              ← Runs on actual IoT device (Raspberry Pi, etc.)
│   ├── src/main.cpp           ← C++ agent (compiles to ./agent)
│   └── agent                  ← Compiled binary (gitignored)
│
├── infra/
│   ├── mqtt_pub.py            ← Simulated device publisher (for testing)
│   ├── mqtt_sub.py            ← Legacy subscriber
│   ├── app.py                 ← Main orchestrator (detects + heals)
│   └── fault_injection/
│       ├── add_delay.sh       ← Injects 2000ms network latency
│       └── remove_delay.sh    ← Removes latency (healing action)
│
├── device_agent.log           ← Live log from agent (gitignored)
└── README.md                  ← This file


## How to Run Everything (Tested & Working)

### Step 1: Start MQTT broker (Mosquitto)
```bash
mosquitto -v

Step 2: Compile & run the C++ agent (on device or laptop)
Bash
cd device_agent
g++ src/main.cpp -o agent -lpthread -std=c++17
./agent

Step 3: Start the Python orchestrator (main brain)
cd infra
python3 app.py

Step 4: Inject a fault & watch it self-heal!
./infra/fault_injection/add_delay.sh      # Inject 2-second delay
# → orchestrator detects → runs remove_delay.sh → system back to normal!
Next 2-Month Roadmap

On-device lightweight anomaly detection (no Python needed)
Auto-restart crashed agent via systemd/watchdog
Configurable healing actions (JSON config)
Web dashboard (FastAPI + React)

⭐ Star | 🔗 Fork | 🤝 Contribute welcomed!
Made with passion by @Immortal-002
