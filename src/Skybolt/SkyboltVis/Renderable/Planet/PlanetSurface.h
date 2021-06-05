/* Copyright 2012-2020 Matthew Reid
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */


#pragma once

#include "SkyboltVis/SkyboltVisFwd.h"
#include "SkyboltVis/OsgBox2.h"
#include "SkyboltVis/VisObject.h"
#include "SkyboltVis/ShadowHelpers.h"
#include "SkyboltVis/Renderable/Planet/Tile/OsgTile.h"
#include "SkyboltVis/Renderable/Planet/Tile/QuadTreeTileLoader.h"
#include "SkyboltVis/Shader/ShaderProgramRegistry.h"
#include <SkyboltCommon/Listenable.h>
#include <SkyboltCommon/Math/QuadTree.h>
#include <osg/Matrix>
#include <osg/MatrixTransform>
#include <osg/Program>
#include <osg/Texture2D>

#include <boost/optional.hpp>

namespace skybolt {
namespace vis {

typedef std::map<skybolt::QuadTreeTileKey, osg::ref_ptr<osg::Image>> TileImageMap;

struct PlanetTileSources
{
	TileSourcePtr elevation;
	TileSourcePtr albedo;
	TileSourcePtr attribute; //!< optional
};

struct PlanetSurfaceConfig
{
	px_sched::Scheduler* scheduler;
	const ShaderPrograms* programs;
	osg::ref_ptr<osg::MatrixTransform> parentTransform; //!< Planet transform
	PlanetTileSources planetTileSources;
	float radius;
	osg::ref_ptr<osg::Texture2D> cloudsTexture; //!< Set to null to disable clouds

	osg::Group* forestGroup; //!< Group under scene root in which to add forests
	float forestGeoVisibilityRange; //!< range beyond which the forestAlbedoMap will be used

	ShadowMaps shadowMaps;
	std::vector<osg::ref_ptr<osg::Texture2D>> albedoDetailMaps;

	int elevationMaxLodLevel = 1;
	int albedoMaxLodLevel = 1;
	bool oceanEnabled = true;
};

struct PlanetSurfaceListener
{
	virtual ~PlanetSurfaceListener() = default;
	virtual void tileAddedToSceneGraph(const skybolt::QuadTreeTileKey& key) {}
	virtual void tileRemovedFromSceneGraph(const skybolt::QuadTreeTileKey& key) {}
};

class PlanetSurface : public skybolt::Listenable<PlanetSurfaceListener>
{
	friend class QuadTreeTileLoader;
public:
	PlanetSurface(const PlanetSurfaceConfig& config);
	~PlanetSurface();

	const PlanetTileSources& getPlanetTileSources() const { return mPlanetTileSources; }

	void updatePreRender(const RenderContext& context);

	skybolt::Listenable<QuadTreeTileLoaderListener>* getTileLoaderListenable() const { return mTileSource.get(); }

private:
	void updateGeometry();

private:
	PlanetTileSources mPlanetTileSources;
	float mRadius;
	std::string mCacheDirectory;

	std::unique_ptr<class QuadTreeTileLoader> mTileSource;
	std::unique_ptr<OsgTileFactory> mOsgTileFactory;
	std::shared_ptr<struct PlanetSubdivisionPredicate> mPredicate;

	osg::ref_ptr<osg::MatrixTransform> mParentTransform;
	osg::ref_ptr<osg::Group> mGroup;
	osg::Group* mForestGroup;

	typedef std::map<skybolt::QuadTreeTileKey, OsgTile> TileNodeMap;
	TileNodeMap mTileNodes;
};

} // namespace vis
} // namespace skybolt
