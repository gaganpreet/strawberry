/*
 * Strawberry Music Player
 * Copyright 2013-2021, Jonas Kvinge <jonas@jkvinge.net>
 *
 * Strawberry is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Strawberry is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Strawberry.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "config.h"

#include <QString>

#include "enginetype.h"

namespace Engine {

Engine::EngineType EngineTypeFromName(const QString &enginename) {
  QString lower = enginename.toLower();
  if (lower == "gstreamer")     return Engine::EngineType::GStreamer;
  else if (lower == "vlc")      return Engine::EngineType::VLC;
  else                          return Engine::EngineType::None;
}

QString EngineName(const Engine::EngineType enginetype) {
  switch (enginetype) {
    case Engine::EngineType::GStreamer:     return QString("gstreamer");
    case Engine::EngineType::VLC:           return QString("vlc");
    case Engine::EngineType::None:
    default:                                return QString("None");
  }
}

QString EngineDescription(Engine::EngineType enginetype) {
  switch (enginetype) {
    case Engine::EngineType::GStreamer:	return QString("GStreamer");
    case Engine::EngineType::VLC:	return QString("VLC");
    case Engine::EngineType::None:
    default:			        return QString("None");

  }
}

}  // namespace Engine
