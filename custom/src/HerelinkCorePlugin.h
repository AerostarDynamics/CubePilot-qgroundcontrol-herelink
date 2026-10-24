#pragma once

#include "HerelinkOptions.h"

#include "QGCApplication.h"
#include "QGCCorePlugin.h"
#include "QGCLoggingCategory.h"

#include <QObject>
#include <QtCore/qapplicationstatic.h>

Q_DECLARE_LOGGING_CATEGORY(HerelinkCorePluginLog)

class HerelinkCorePlugin : public QGCCorePlugin
{
    Q_OBJECT

public:
    HerelinkCorePlugin(QObject* parent = nullptr);

    static HerelinkCorePlugin* instance();

    Q_PROPERTY(bool isHerelink READ isHerelink CONSTANT)
    bool isHerelink (void) const { return true; }

    // Overrides from QGCCorePlugin
    QGCOptions* options                                (void) override { return qobject_cast<QGCOptions*>(_herelinkOptions); }
    bool        overrideSettingsGroupVisibility        (const QString& name) override;
    bool        adjustSettingMetaData                  (const QString& settingsGroup, FactMetaData& metaData) override;
    void        factValueGridCreateDefaultSettings     (FactValueGrid* factValueGrid) override;
    // Aerostar logo in the fly-view toolbar: white lettering for the dark (indoor) palette, black for the light one
    QString     brandImageIndoor                       (void) const override { return QStringLiteral("/custom/img/AerostarLogoWhite.png"); }
    QString     brandImageOutdoor                      (void) const override { return QStringLiteral("/custom/img/AerostarLogoBlack.png"); }


private slots:
    void _activeVehicleChanged(Vehicle* activeVehicle);

private:
    HerelinkOptions* _herelinkOptions = nullptr;
};

Q_APPLICATION_STATIC(HerelinkCorePlugin, _herelinkCorePluginInstance);
