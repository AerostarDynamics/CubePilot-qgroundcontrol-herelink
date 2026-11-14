#pragma once

#include "QGCOptions.h"

class HerelinkCorePlugin;

class HerelinkOptions : public QGCOptions
{
public:
    HerelinkOptions(HerelinkCorePlugin* plugin, QObject* parent = NULL);

    // QGCOptions overrides
    bool wifiReliableForCalibration () const override { return true; }
    bool showFirmwareUpgrade        () const override { return false; }
    bool multiVehicleEnabled        () const override { return false; }
    bool allowJoystickSelection     () const override { return false; }
    // Aerostar (user, 14 Nov 2026): no "Vehicle is not running latest stable firmware" pop-up when an aircraft connects -
    // Aerostar drones run Aerostar's own ArduPilot builds
    bool checkFirmwareVersion       () const override { return false; }
};
