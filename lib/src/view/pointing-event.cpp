/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QGuiApplication>
#include <QMouseEvent>
#include <QTabletEvent>
#include "config.hpp"
#include "view/pointing-event.hpp"

ViewPointingEvent::ViewPointingEvent (const QMouseEvent& event)
  : _modifiers (event.modifiers ())
  , _pressEvent (event.type () == QEvent::MouseButtonPress)
  , _moveEvent (event.type () == QEvent::MouseMove)
  , _releaseEvent (event.type () == QEvent::MouseButtonRelease)
  , _leftButton (this->_moveEvent ? event.buttons () == Qt::LeftButton
                                  : event.button () == Qt::LeftButton)
  , _middleButton (this->_moveEvent ? event.buttons () == Qt::MiddleButton
                                    : event.button () == Qt::MiddleButton)
  , _ivec2 (glm::ivec2 (event.x (), event.y ()))
  , _intensity (1.0f)
{
}

ViewPointingEvent::ViewPointingEvent (const Config& config, const QTabletEvent& event)
  : _modifiers (QGuiApplication::queryKeyboardModifiers ())
  , _pressEvent (event.type () == QEvent::TabletPress)
  , _moveEvent (event.type () == QEvent::TabletMove)
  , _releaseEvent (event.type () == QEvent::TabletRelease)
  , _leftButton (this->_moveEvent ? event.buttons () == Qt::LeftButton
                                  : event.button () == Qt::LeftButton)
  , _middleButton (this->_moveEvent ? event.buttons () == Qt::MiddleButton
                                    : event.button () == Qt::MiddleButton)
  , _ivec2 (glm::ivec2 (event.x (), event.y ()))
  , _intensity (config.get<float> ("editor/tablet-pressure-intensity") * event.pressure ())
{
}

bool ViewPointingEvent::valid () const
{
  return this->pressEvent () || this->moveEvent () || this->releaseEvent ();
}
