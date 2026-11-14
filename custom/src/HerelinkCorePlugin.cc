#include "HerelinkCorePlugin.h"

#include "AutoConnectSettings.h"
#include "VideoSettings.h"
#include "AppSettings.h"
#include "QGCApplication.h"
#include "MultiVehicleManager.h"
#include "JoystickManager.h"
#include "HorizontalFactValueGrid.h"
#include "InstrumentValueData.h"
#include "FactValueGrid.h"

#include <QtCore/QTimer>
#include <QtGui/QGuiApplication>
#ifdef Q_OS_ANDROID
#include <QtCore/QCoreApplication>  // QNativeInterface::QAndroidApplication
#include <QtQml/QQmlApplicationEngine>
#include <QtQuick/QQuickWindow>
#endif

#include <list>

QGC_LOGGING_CATEGORY(HerelinkCorePluginLog, "HerelinkCorePluginLog")

HerelinkCorePlugin* HerelinkCorePlugin::instance()
{
    return _herelinkCorePluginInstance();
}

HerelinkCorePlugin::HerelinkCorePlugin(QObject* parent)
    : QGCCorePlugin(parent)
{
    _herelinkOptions = new HerelinkOptions(this, nullptr);
    // Pop-up titles say "Aerostar GCS". The application NAME stays "QGroundControl-v5 Daily": it is the settings
    // location, and changing it would reset every remote's saved settings.
    QGuiApplication::setApplicationDisplayName(QStringLiteral("Aerostar GCS"));

    connect(MultiVehicleManager::instance(), &MultiVehicleManager::activeVehicleChanged, this, &HerelinkCorePlugin::_activeVehicleChanged);
}


void HerelinkCorePlugin::createRootWindow(QQmlApplicationEngine* qmlEngine)
{
    QGCCorePlugin::createRootWindow(qmlEngine);
#ifdef Q_OS_ANDROID
    // The main window takes ~5 s to load on the remote, so the logo is already up long enough (user, 6 Nov 2026):
    // hide it as soon as the main window has drawn its first frame (no black gap), with a short fade.
    // Fallback timer in case no frame signal arrives.
    auto hide = []() { QNativeInterface::QAndroidApplication::hideSplashScreen(kSplashFadeMs); };
    QQuickWindow* window = qmlEngine->rootObjects().isEmpty() ? nullptr : qobject_cast<QQuickWindow*>(qmlEngine->rootObjects().first());
    if (window) {
        connect(window, &QQuickWindow::frameSwapped, this, hide, Qt::SingleShotConnection);
    }
    QTimer::singleShot(window ? 3000 : 300, this, hide);
#endif
}

bool HerelinkCorePlugin::overrideSettingsGroupVisibility(const QString& name)
{
    // Hide all AutoConnect settings
    return name != AutoConnectSettings::name;
}

bool HerelinkCorePlugin::adjustSettingMetaData(const QString& settingsGroup, FactMetaData& metaData)
{
    if (settingsGroup == AppSettings::settingsGroup) {
        //-- Default herelink fontsize of 10, it is a nice starting point
        if (metaData.name() == AppSettings::appFontPointSizeName) {
            uint32_t fontSize = 10;
            metaData.setRawDefaultValue(fontSize);
            // Show setting in ui
            return true;
        }
        //-- Default Palette Dark
        if (metaData.name() == AppSettings::indoorPaletteName) {
            QVariant outdoorPalette;
            outdoorPalette = 1;
            metaData.setRawDefaultValue(outdoorPalette);
            // Show setting in ui
            return true;
        }
    }
    if (settingsGroup == AutoConnectSettings::settingsGroup) {
        // We have to adjust the Herelink UDP autoconnect settings for the AirLink
        if (metaData.name() == AutoConnectSettings::udpListenPortName) {
            metaData.setRawDefaultValue(14551);
        } else if (metaData.name() == AutoConnectSettings::udpTargetHostIPName) {
            metaData.setRawDefaultValue(QStringLiteral("127.0.0.1"));
        } else if (metaData.name() == AutoConnectSettings::udpTargetHostPortName) {
            metaData.setRawDefaultValue(15552);
        } else {
            // Disable all the other autoconnect types
            const std::list<const char *> disabledAndHiddenSettings = {
                AutoConnectSettings::autoConnectPixhawkName,
                AutoConnectSettings::autoConnectSiKRadioName,
                AutoConnectSettings::autoConnectRTKGPSName,
                AutoConnectSettings::autoConnectLibrePilotName,
                AutoConnectSettings::autoConnectNmeaPortName,
                AutoConnectSettings::autoConnectZeroConfName,
            };
            for (const char * disabledAndHiddenSetting : disabledAndHiddenSettings) {
                if (disabledAndHiddenSetting == metaData.name()) {
                    metaData.setRawDefaultValue(false);
                }
            }
        }
    } else if (settingsGroup == VideoSettings::settingsGroup) {
        if (metaData.name() == VideoSettings::rtspTimeoutName) {
            metaData.setRawDefaultValue(60);
        } else if (metaData.name() == VideoSettings::videoSourceName) {
            metaData.setRawDefaultValue(VideoSettings::videoSourceHerelinkAirUnit);
        }
    } else if (settingsGroup == AppSettings::settingsGroup) {
        if (metaData.name() == AppSettings::androidSaveToSDCardName) {
            metaData.setRawDefaultValue(true);
        }
    }

    return true; // Show all settings in ui
}

void HerelinkCorePlugin::_activeVehicleChanged(Vehicle* activeVehicle)
{
    if (activeVehicle) {
        QString herelinkButtonsJoystickName("gpio-keys");

        auto joystickManager = JoystickManager::instance();
        if (joystickManager->activeJoystickName() != herelinkButtonsJoystickName) {
            if (!joystickManager->setActiveJoystickName(herelinkButtonsJoystickName)) {
                qgcApp()->showAppMessage("Warning: Herelink buttton setup failed. Buttons will not work.");
                return;
            }           
        }
        activeVehicle->setJoystickEnabled(true);
    }
}

// Same as original, only we set font size to medium by default for Herelink
void HerelinkCorePlugin::factValueGridCreateDefaultSettings(FactValueGrid* factValueGrid)
{
    bool        includeFWValues = factValueGrid->vehicleClass() == QGCMAVLink::VehicleClassFixedWing || factValueGrid->vehicleClass() == QGCMAVLink::VehicleClassVTOL || factValueGrid->vehicleClass() == QGCMAVLink::VehicleClassAirship;

    factValueGrid->setFontSize(FactValueGrid::MediumFontSize);

    factValueGrid->appendColumn();
    factValueGrid->appendColumn();
    factValueGrid->appendColumn();
    if (includeFWValues) {
        factValueGrid->appendColumn();
    }
    factValueGrid->appendRow();

    int                 rowIndex    = 0;
    QmlObjectListModel* column      = factValueGrid->columns()->value<QmlObjectListModel*>(0);

    InstrumentValueData* value = column->value<InstrumentValueData*>(rowIndex++);
    value->setFact("Vehicle", "AltitudeRelative");
    value->setIcon("arrow-thick-up.svg");
    value->setText(value->fact()->shortDescription());
    value->setShowUnits(true);

    value = column->value<InstrumentValueData*>(rowIndex++);
    value->setFact("Vehicle", "DistanceToHome");
    value->setIcon("bookmark copy 3.svg");
    value->setText(value->fact()->shortDescription());
    value->setShowUnits(true);

    rowIndex    = 0;
    column      = factValueGrid->columns()->value<QmlObjectListModel*>(1);

    value = column->value<InstrumentValueData*>(rowIndex++);
    value->setFact("Vehicle", "ClimbRate");
    value->setIcon("arrow-simple-up.svg");
    value->setText(value->fact()->shortDescription());
    value->setShowUnits(true);

    value = column->value<InstrumentValueData*>(rowIndex++);
    value->setFact("Vehicle", "GroundSpeed");
    value->setIcon("arrow-simple-right.svg");
    value->setText(value->fact()->shortDescription());
    value->setShowUnits(true);


    if (includeFWValues) {
        rowIndex    = 0;
        column      = factValueGrid->columns()->value<QmlObjectListModel*>(2);

        value = column->value<InstrumentValueData*>(rowIndex++);
        value->setFact("Vehicle", "AirSpeed");
        value->setText("AirSpd");
        value->setShowUnits(true);

        value = column->value<InstrumentValueData*>(rowIndex++);
        value->setFact("Vehicle", "ThrottlePct");
        value->setText("Thr");
        value->setShowUnits(true);
    }

    rowIndex    = 0;
    column      = factValueGrid->columns()->value<QmlObjectListModel*>(includeFWValues ? 3 : 2);

    value = column->value<InstrumentValueData*>(rowIndex++);
    value->setFact("Vehicle", "FlightTime");
    value->setIcon("timer.svg");
    value->setText(value->fact()->shortDescription());
    value->setShowUnits(false);

    value = column->value<InstrumentValueData*>(rowIndex++);
    value->setFact("Vehicle", "FlightDistance");
    value->setIcon("travel-walk.svg");
    value->setText(value->fact()->shortDescription());
    value->setShowUnits(true);
}
