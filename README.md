# Self-Healing IoT System 🛠️⚡

An autonomous IoT framework that detects faults in real-time and self-heals without human intervention.

## Current Features (as of today)
- C++ device agent running on edge devices
- MQTT-based communication infrastructure (Python)
- Fault injection scripts for testing self-healing logic
- Real-time logging (`device_agent.log`)

## Project Structure
self_healing_iot/
├── device_agent/              ← C++ agent running on IoT devices
│   ├── src/main.cpp           ← Core agent logic
│   └── device_agent.log       ← Live logs (gitignored)
│
├── infra/                     ← Infrastructure & testing scripts
│   ├── mqtt_pub.py            ← Publishes device telemetry & heartbeats
│   ├── mqtt_sub.py            ← Listens and triggers healing actions
│   └── fault_injection/
│       ├── add_delay.sh       ← Simulates network latency fault
│       └── remove_delay.sh    ← Recovers from the fault (self-healing demo)
│
└── README.md                  ← You’re reading it!

## How to Run (Quick Start)
```bash
# Start MQTT broker (e.g., Mosquitto) separately
# Then:
cd infra
python3 mqtt_sub.py &   # Run subscriber in background
python3 mqtt_pub.py     # Simulate device

# In another terminal → inject fault
./infra/fault_injection/add_delay.sh
# Watch the system detect + self-heal automatically!

Next 2 Months Roadmap

Add anomaly detection using lightweight ML on-device
Implement auto-restart of crashed agents
Add watchdog + rollback mechanisms
Web dashboard for monitoring

Open for contributions! ⭐ the repo and fork if you like it!
Made with ❤️ by @Immortal-002
