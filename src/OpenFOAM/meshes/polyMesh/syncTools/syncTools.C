/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*----------------------------------------------------------------------------*/

#include "syncTools.H"

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

// Determines for every point whether it is coupled and if so sets only one.
Foam::PackedBoolList Foam::syncTools::getMasterPoints(const polyMesh& mesh)
{
    PackedBoolList isMasterPoint(mesh.nPoints());
    PackedBoolList donePoint(mesh.nPoints());

    const globalMeshData& globalData = mesh.globalData();
    const labelList& meshPoints = globalData.coupledPatch().meshPoints();
    const labelListList& slaves = globalData.globalPointSlaves();
    const labelListList& transformedSlaves =
            globalData.globalPointTransformedSlaves();

    forAll(meshPoints, coupledPointI)
    {
        label meshPointI = meshPoints[coupledPointI];
        if
        (
            (
                slaves[coupledPointI].size()
              + transformedSlaves[coupledPointI].size()
            )
          > 0
        )
        {
            isMasterPoint[meshPointI] = true;
        }
        donePoint[meshPointI] = true;
    }


    // Do all other points
    // ~~~~~~~~~~~~~~~~~~~

    forAll(donePoint, pointI)
    {
        if (!donePoint[pointI])
        {
            isMasterPoint[pointI] = true;
        }
    }

    return isMasterPoint;
}


// Determines for every edge whether it is coupled and if so sets only one.
Foam::PackedBoolList Foam::syncTools::getMasterEdges(const polyMesh& mesh)
{
    PackedBoolList isMasterEdge(mesh.nEdges());
    PackedBoolList doneEdge(mesh.nEdges());

    const globalMeshData& globalData = mesh.globalData();
    const labelList& meshEdges = globalData.coupledPatchMeshEdges();
    const labelListList& slaves = globalData.globalEdgeSlaves();
    const labelListList& transformedSlaves =
        globalData.globalEdgeTransformedSlaves();

    forAll(meshEdges, coupledEdgeI)
    {
        label meshEdgeI = meshEdges[coupledEdgeI];
        if
        (
            (
                slaves[coupledEdgeI].size()
              + transformedSlaves[coupledEdgeI].size()
            )
          > 0
        )
        {
            isMasterEdge[meshEdgeI] = true;
        }
        doneEdge[meshEdgeI] = true;
    }


    // Do all other edges
    // ~~~~~~~~~~~~~~~~~~

    forAll(doneEdge, edgeI)
    {
        if (!doneEdge[edgeI])
        {
            isMasterEdge[edgeI] = true;
        }
    }

    return isMasterEdge;
}


// Determines for every face whether it is coupled and if so sets only one.
Foam::PackedBoolList Foam::syncTools::getMasterFaces(const polyMesh& mesh)
{
    PackedBoolList isMasterFace(mesh.nFaces(), 1);

    const polyBoundaryMesh& patches = mesh.boundaryMesh();

    forAll(patches, patchI)
    {
        if (patches[patchI].coupled())
        {
            const coupledPolyPatch& pp =
                refCast<const coupledPolyPatch>(patches[patchI]);

            if (!pp.owner())
            {
                forAll(pp, i)
                {
                    isMasterFace.unset(pp.start()+i);
                }
            }
        }
    }

    return isMasterFace;
}


// ************************************************************************* //
