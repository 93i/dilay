/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "adjacent-iterator.hpp"
#include "affected-faces.hpp"
#include "intersection.hpp"
#include "partial-action/smooth.hpp"
#include "primitive/ray.hpp"
#include "primitive/triangle.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

namespace {
  bool relaxableEdge (const WingedEdge& edge) {
    const int v1  = int (edge.vertex1Ref ().valence ());
    const int v2  = int (edge.vertex2Ref ().valence ());
    const int v3  = int (edge.vertexRef (edge.leftFaceRef  (), 2).valence ());
    const int v4  = int (edge.vertexRef (edge.rightFaceRef (), 2).valence ());

    const int pre  = glm::abs (v1-6)   + glm::abs (v2-6)   + glm::abs (v3-6)   + glm::abs (v4-6);
    const int post = glm::abs (v1-6-1) + glm::abs (v2-6-1) + glm::abs (v3-6+1) + glm::abs (v4-6+1);

    return (v1 > 3) && (v2 > 3) && (post < pre);
  }

  void flipEdge (WingedMesh& mesh, WingedEdge& edge) {
    assert (edge.leftFaceRef  ().numEdges () == 3);
    assert (edge.rightFaceRef ().numEdges () == 3);

    WingedVertex* v1 = edge.vertex1          ();
    WingedVertex* v2 = edge.vertex2          ();
    WingedFace*   lf = edge.leftFace         ();
    WingedFace*   rf = edge.rightFace        ();
    WingedEdge*   lp = edge.leftPredecessor  ();
    WingedEdge*   ls = edge.leftSuccessor    ();
    WingedEdge*   rp = edge.rightPredecessor ();
    WingedEdge*   rs = edge.rightSuccessor   ();
    WingedVertex* v3 = edge.vertex           (*lf, 2);
    WingedVertex* v4 = edge.vertex           (*rf, 2);

#ifndef NDEBUG
    const unsigned int valence1 = v1->valence ();
    const unsigned int valence2 = v2->valence ();
    const unsigned int valence3 = v3->valence ();
    const unsigned int valence4 = v4->valence ();
#endif
    assert (valence1 > 3);
    assert (valence2 > 3);

    lf->edge (&edge);
    rf->edge (&edge);

    v1->edge (lp);
    v2->edge (rp);

    ls->face (*lf, rf);
    rs->face (*rf, lf);

    lp->predecessor (*lf, &edge);
    lp->successor   (*lf, rs);
    ls->predecessor (*rf, rp);
    ls->successor   (*rf, &edge);
    rp->predecessor (*rf, &edge);
    rp->successor   (*rf, ls);
    rs->predecessor (*lf, lp);
    rs->successor   (*lf, &edge);

    edge.setGeometry (v4, v3, lf, rf, rs, lp, ls, rp);

    edge.leftFaceRef  ().writeIndices (mesh);
    edge.rightFaceRef ().writeIndices (mesh);

    assert (edge.leftFaceRef  ().numEdges () == 3);
    assert (edge.rightFaceRef ().numEdges () == 3);
    assert (edge.vertex1Ref ().valence () == valence4 + 1);
    assert (edge.vertex2Ref ().valence () == valence3 + 1);
  }
}

namespace PartialAction {
  void smooth (WingedMesh& mesh, const VertexPtrSet& vertices, AffectedFaces& affectedFaces) {
    for (WingedVertex* v : vertices) {
      unsigned int valence = 0;
      unsigned int numTriangles = 0;
      glm::vec3 center = glm::vec3 (0.0f);
      glm::vec3 normal = glm::vec3 (0.0f);

      for (WingedEdge& e : v->adjacentEdges ()) {
        WingedFace& face = e.isVertex1 (*v) ? e.rightFaceRef () : e.leftFaceRef ();
        const PrimTriangle triangle = face.triangle (mesh);

        center += e.otherVertexRef (*v).position (mesh);
        valence++;

        if (triangle.isDegenerated () == false) {
          normal += triangle.normal ();
          numTriangles++;
        }
        affectedFaces.insert (face);
      }
      center /= float (valence);
      normal /= float (numTriangles);

      const glm::vec3 delta = center - v->position (mesh);
      const glm::vec3 tangentialPos = center - (normal * glm::dot (normal, delta));

      v->position (mesh,tangentialPos);
    }
  }

  void relaxEdge (WingedMesh& mesh, WingedEdge& edge, AffectedFaces& affectedFaces) {
    if (relaxableEdge (edge)) {
      affectedFaces.insert (edge.leftFaceRef  ());
      affectedFaces.insert (edge.rightFaceRef ());

      flipEdge (mesh, edge);
    }
  }
}
