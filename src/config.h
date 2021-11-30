#ifndef CONFIG_H
#define CONFIG_H

const int STEPS_PER_ROTATION = 200;
const double ROTATIONS_TO_CLOSE = 21;
const int STEPS_TO_CLOSE = STEPS_PER_ROTATION * ROTATIONS_TO_CLOSE;
const boolean CLOCKWISE_CLOSE = false;
int offset = 0;
const double MAX_SPEED = STEPS_PER_ROTATION;
const double ACCELERATION = STEPS_PER_ROTATION;

// 0 is open
int getPctOpenPosition(const int pct)
{
    return (1 - pct / 100.0) * STEPS_TO_CLOSE + offset;
}

void updateOffset(const boolean isOpen, int currentPosition)
{
    if (isOpen)
    {
        offset = currentPosition;
    }
    else
    {
        offset = currentPosition + (getPctOpenPosition(100) - getPctOpenPosition(0));
    }
}

int getCurrentPctOpen(const int currentPosition)
{
    return round((1 - double(currentPosition - offset) / double(STEPS_TO_CLOSE)) * 100.0);
}

#endif