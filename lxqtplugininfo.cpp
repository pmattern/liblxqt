/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */


#include "lxqtplugininfo.h"
#include <QObject>
#include <QFileInfo>
#include <QDir>
#include <QTranslator>
#include <QCoreApplication>
#include <QLibrary>
#include <QDebug>

using namespace LXQt;

/************************************************

 ************************************************/
PluginInfo::PluginInfo():
    XdgDesktopFile()
{

}


/************************************************

 ************************************************/
bool PluginInfo::load(const QString& fileName)
{
    XdgDesktopFile::load(fileName);
    mId = QFileInfo(fileName).completeBaseName();
    return isValid();
}


/************************************************

 ************************************************/
bool PluginInfo::isValid() const
{
    return XdgDesktopFile::isValid();
}


/************************************************

 ************************************************/
QLibrary* PluginInfo::loadLibrary(const QString& libDir) const
{
    QString baseName, path;
    QFileInfo fi = QFileInfo(fileName());
    path = fi.canonicalPath();
    baseName = value("X-LXQt-Library", fi.completeBaseName()).toString();

    QString soPath = QDir(libDir).filePath(QString("lib%2.so").arg(baseName));
    QLibrary* library = new QLibrary(soPath);

    if (!library->load())
    {
        qWarning() << QString("Can't load plugin lib \"%1\"").arg(soPath) << library->errorString();
        delete library;
        return 0;
    }

    QString locale = QLocale::system().name();
    QTranslator* translator = new QTranslator(library);

    translator->load(QString("%1/%2/%2_%3.qm").arg(path, baseName, locale));
    qApp->installTranslator(translator);

    return library;
}


/************************************************

 ************************************************/
PluginInfoList PluginInfo::search(const QStringList& desktopFilesDirs, const QString& serviceType, const QString& nameFilter)
{
    QList<PluginInfo> res;
    QSet<QString> processed;

    foreach (const QString &desktopFilesDir, desktopFilesDirs)
    {
        QDir dir(desktopFilesDir);
        QFileInfoList files = dir.entryInfoList(QStringList(nameFilter), QDir::Files | QDir::Readable);
        foreach (const QFileInfo &file, files)
        {
            if (processed.contains(file.fileName()))
                continue;

            processed << file.fileName();

            PluginInfo item;
            item.load(file.canonicalFilePath());

            if (item.isValid() && item.serviceType() == serviceType)
                res.append(item);
        }
    }
    return res;
}


/************************************************

 ************************************************/
PluginInfoList PluginInfo::search(const QString& desktopFilesDir, const QString& serviceType, const QString& nameFilter)
{
    return search(QStringList(desktopFilesDir), serviceType, nameFilter);
}


/************************************************

 ************************************************/
QDebug operator<<(QDebug dbg, const LXQt::PluginInfo &pluginInfo)
{
    dbg.nospace() << QString("%1").arg(pluginInfo.id());
    return dbg.space();
}


/************************************************

 ************************************************/
QDebug operator<<(QDebug dbg, const LXQt::PluginInfo * const pluginInfo)
{
    return operator<<(dbg, *pluginInfo);
}


/************************************************

 ************************************************/
QDebug operator<<(QDebug dbg, const PluginInfoList& list)
{
    dbg.nospace() << '(';
    for (int i=0; i<list.size(); ++i)
    {
        if (i) dbg.nospace() << ", ";
        dbg << list.at(i);
    }
    dbg << ')';
    return dbg.space();
}


/************************************************

 ************************************************/
QDebug operator<<(QDebug dbg, const PluginInfoList* const pluginInfoList)
{
    return operator<<(dbg, *pluginInfoList);
}
