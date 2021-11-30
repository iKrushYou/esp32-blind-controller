#include "Arduino.h"

#ifndef HTML_H
#define HTML_H

const PROGMEM char index_html[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name=\"viewport\" content=\"width=device-width\">
<body>

<h1>
  Blind Automation Server
</h1>
<div style="margin-bottom: 30px">
  <button onclick='onSetPositionPct(100)'>
    Open
  </button>
  <button onclick='onSetPositionPct(0)'>
    Close
  </button>
</div>
<div style="margin-bottom: 30px">
    <p>config</p>
    <ul>
        <li>STEPS_PER_ROTATION: %STEPS_PER_ROTATION%</li>
        <li>ROTATIONS_TO_CLOSE: %ROTATIONS_TO_CLOSE%</li>
        <li>STEPS_TO_CLOSE: %STEPS_TO_CLOSE%</li>
        <li>CLOCKWISE_CLOSE: %CLOCKWISE_CLOSE%</li>
    </ul>
</div>
<div style="margin-bottom: 30px">
  <label for="positionInput" style="display: block">
    Current Position
  </label>
  <input id="positionInput" value="%CURRENT_POSITION%" />
  <button onclick="handleSetPosition()">
    Set Position
  </button>
</div>
<div style="margin-bottom: 30px">
  <label for="positionInput" style="display: block">
    Current Open Percentage
  </label>
  <input id="pctInput" value="%CURRENT_PCT_OPEN%" />
  <button onclick="handleSetPct()">
    Set Percentage
  </button>
</div>
<div style="margin-bottom: 30px">
  <label for="positionInput" style="display: block">
    Offset
  </label>
  <input id="positionInput" value="%OFFSET%" disabled/>
  <button onclick="handleZero('open')">
    Blind is Open
  </button>
  <button onclick="handleZero('closed')">
    Blind is Closed
  </button>
</div>
<div>
  <button onclick="handleStep(-%STEPS_PER_ROTATION%)">
    Step Closed
  </button>
  <button onclick="handleStep(%STEPS_PER_ROTATION%)">
    Step Open
  </button>
</div>

<script>
  async function onSetPositionPct(pct) {
    await fetch(`/api/position?pct=${pct}`, {
      method: "PUT"
    });
  }

  async function handleSetPosition() {
    const position = document.getElementById("positionInput").value;
    if (position.length === 0) return;
    await fetch(`/api/position?value=${position}`, {
      method: "PUT"
    });
  }

  async function handleSetPct() {
    const pct = document.getElementById("pctInput").value;
    if (pct.length === 0) return;
    await fetch(`/api/position?pct=${pct}`, {
      method: "PUT"
    });
  }

  async function handleZero(openClosed) {
    await fetch(`/api/zero?position=${openClosed}`, {
      method: "PUT"
    });
  }

  async function handleStep(distance) {
    await fetch(`/api/step?distance=${distance}`, {
      method: "PUT"
    });
  }
</script>

</body>
</html>
)rawliteral";

#endif