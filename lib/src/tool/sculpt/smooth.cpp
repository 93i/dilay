#include <QCheckBox>
#include <QDoubleSpinBox>
#include "cache.hpp"
#include "sculpt-brush.hpp"
#include "tools.hpp"
#include "view/properties.hpp"
#include "view/tool-tip.hpp"
#include "view/util.hpp"

struct ToolSculptSmooth::Impl {
  ToolSculptSmooth* self;

  Impl (ToolSculptSmooth* s) 
    : self (s) 
  {}

  void runSetupBrush (SculptBrush& brush) {
    auto& params = brush.parameters <SBSmoothParameters> ();

    params.intensity (this->self->cache ().get <float> ("intensity", 0.5f));
    params.relaxOnly (this->self->cache ().get <bool>  ("relax-only", false));
  }

  void runSetupCursor (ViewCursor&) {}

  void runSetupProperties (ViewPropertiesPart& properties) {
    auto& params = this->self->brush ().parameters <SBSmoothParameters> ();

    QDoubleSpinBox& intensityEdit = ViewUtil::spinBox ( 0.1f, params.intensity ()
                                                      , 1.0f, 0.1f );
    ViewUtil::connect (intensityEdit, [this,&params] (float i) {
      params.intensity (i);
      this->self->cache ().set ("intensity", i);
    });
    properties.add (QObject::tr ("Intensity"), intensityEdit);

    QCheckBox& relaxEdit = ViewUtil::checkBox (QObject::tr ("Relax only"), params.relaxOnly ());
    ViewUtil::connect (relaxEdit, [this,&params] (bool r) {
      params.relaxOnly (r);
      this->self->cache ().set ("relax-only", r);
    });
    properties.add (relaxEdit);
  }

  void runSetupToolTip (ViewToolTip& toolTip) {
    toolTip.add ( ViewToolTip::MouseEvent::Left, QObject::tr ("Drag to smooth"));
  }

  ToolResponse runMouseMoveEvent (const QMouseEvent& e) {
    this->self->carvelikeStroke (e);
    return ToolResponse::Redraw;
  }

  ToolResponse runMousePressEvent (const QMouseEvent& e) {
    this->self->carvelikeStroke (e);
    return ToolResponse::Redraw;
  }
};

DELEGATE_TOOL_SCULPT (ToolSculptSmooth)
