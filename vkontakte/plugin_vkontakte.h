/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2010-11-15
 * Description : a kipi plugin to export images to VKontakte web service
 *
 * Copyright (C) 2010 by Roman Tsisyk <roman at tsisyk dot com>
 * Copyright (C) 2011 by Alexander Potashev <aspotashev at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef PLUGIN_VKONTAKTE_H
#define PLUGIN_VKONTAKTE_H

// KDE includes

#include <kaction.h>

// Libkipi includes

#include <libkipi/plugin.h>

// Local includes

#include "vkwindow.h"

using namespace KIPI;

namespace KIPIVkontaktePlugin
{

class Plugin_Vkontakte : public Plugin
{
    Q_OBJECT

public:

    Plugin_Vkontakte(QObject* const parent, const QVariantList& args);
    ~Plugin_Vkontakte();

    virtual Category category(KAction *action) const;
    virtual void setup(QWidget*);

public Q_SLOTS:

    void slotExport();
//    void slotImport();

private:

    KAction*         m_actionExport;
    VkontakteWindow* m_dlgExport;
};

} // namespace KIPIVkontaktePlugin

#endif // PLUGIN_VKONTAKTE_H
