const uint8_t CAMERA_TRIG_PIN       = 15;   // Output to camera trigger input
const uint8_t EXPOSURE_READY_PIN    = 17;   // Input from camera exposure active signal
const uint8_t LED1_PIN              = 21;   // Output to LED 1
const uint8_t LED2_PIN              = 22;   // Output to LED 2

const unsigned long TRIG_PULSE_US       = 50;
const unsigned long CAPTURE_INTERVAL_MS = 200;
const unsigned int NUM_MODES            = 2;

volatile bool captureFinished = false;

// Interrupt diagnostics
// volatile unsigned long interruptCount = 0;
volatile unsigned long lastInterruptMicros = 0;
// volatile unsigned long interruptIntervals[10];
// volatile unsigned int interruptIndex = 0;

unsigned int currentFrameNum = 1;
bool toggleState = false;

unsigned long lastCaptureExpectedStartTime = 0;


// ============================================================
// INTERRUPT
// ============================================================

void IRAM_ATTR onCaptureFinished() {
    unsigned long now = micros();
    if (now - lastInterruptMicros < 2 * TRIG_PULSE_US) {
        return;
    }

    // interruptCount++;

    // Store time since previous interrupt
    // if (interruptIndex < 10) {
    //     interruptIntervals[interruptIndex] =
    //         now - lastInterruptMicros;

    //     interruptIndex++;
    // }

    lastInterruptMicros = now;

    captureFinished = true;
}


// ============================================================
// LED MODES
// ============================================================

void mode_1() {
    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, HIGH);
}

void mode_2() {
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, LOW);
}


// ============================================================
// CAMERA TRIGGER
// ============================================================

void triggerCamera() {
    digitalWrite(CAMERA_TRIG_PIN, HIGH);
    delayMicroseconds(TRIG_PULSE_US);
    digitalWrite(CAMERA_TRIG_PIN, LOW);
}

// ============================================================
// SETUP
// ============================================================

void setup() {
    Serial.begin(115200);

    pinMode(CAMERA_TRIG_PIN, OUTPUT);
    pinMode(LED1_PIN, OUTPUT);
    pinMode(LED2_PIN, OUTPUT);

    digitalWrite(CAMERA_TRIG_PIN, LOW);
    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, LOW);

    pinMode(EXPOSURE_READY_PIN, INPUT_PULLUP);

    attachInterrupt(
        digitalPinToInterrupt(EXPOSURE_READY_PIN),
        onCaptureFinished,
        RISING
    );

    mode_1();

    lastCaptureExpectedStartTime = millis();

    Serial.println("Setup completed.");
}


// ============================================================
// LOOP
// ============================================================

void loop() {

    // --------------------------------------------------------
    // Handle exposure completion
    // --------------------------------------------------------

    if (captureFinished) {

        // Disable interrupts briefly while copying diagnostic data
        noInterrupts();

        captureFinished = false;

        // unsigned long count = interruptCount;
        // unsigned int index = interruptIndex;

        // unsigned long intervals[10];

        // for (unsigned int i = 0; i < index; i++) {
        //     intervals[i] = interruptIntervals[i];
        // }

        interrupts();
        toggleState = !toggleState;
        if (toggleState) {
            mode_2();
        } else {
            mode_1();
        }

        if (currentFrameNum < NUM_MODES) {
            currentFrameNum++;
            triggerCamera();
        }


        // ----------------------------------------------------
        // Print interrupt diagnostics
        // ----------------------------------------------------

        // Serial.print("Interrupt count: ");
        // Serial.println(count);

        // Serial.print("Interrupt intervals: ");

        // for (unsigned int i = 0; i < index; i++) {

        //     Serial.print(intervals[i]);

        //     Serial.print(" us");

        //     if (i < index - 1) {
        //         Serial.print(", ");
        //     }
        // }

        // Serial.println();
    }


    // --------------------------------------------------------
    // Start new capture sequence
    // --------------------------------------------------------

    if (millis() - lastCaptureExpectedStartTime >= CAPTURE_INTERVAL_MS) {

        currentFrameNum = 1;
        toggleState = false;

        lastCaptureExpectedStartTime += CAPTURE_INTERVAL_MS;

        // Reset interrupt diagnostics
        noInterrupts();

        // interruptCount = 0;
        // interruptIndex = 0;
        lastInterruptMicros = 0;

        interrupts();

        mode_1();
        lastInterruptMicros = micros();
        triggerCamera();
    }
}