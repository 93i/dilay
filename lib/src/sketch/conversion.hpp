/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_SKETCH_CONVERSION
#define DILAY_SKETCH_CONVERSION

class Mesh;
class SketchMesh;

namespace SketchConversion
{
  Mesh convert (const SketchMesh&, float);
};

#endif
