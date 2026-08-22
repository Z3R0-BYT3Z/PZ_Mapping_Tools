# PZTools documentation index

This is the source-tree index for the maintained WorldEd, TileZed, and
BuildingEd documentation. A compiled release contains the same entry points
under `docs/index.html`.

## Start here

- [PZTools user guide](docs/TileZed/PZToolsGuide.html)
- [Feature reference](docs/Feature-Reference.md)
- [Troubleshooting FAQ](docs/FAQ.md)
- [Logs and useful issue reports](docs/Diagnostics-and-Logs.md)
- [Configuration files](docs/PZTools-Configuration-Files.md)
- [Current release changes](RELEASE_CHANGELOG.md)
- [Build instructions](BUILDING.md)
- [Feature-level provenance](FEATURE_PROVENANCE.md)
- [Upstream history and source provenance](UPSTREAM-HISTORY.md)

## WorldEd

| Subject | Reference |
|---|---|
| Project health, missing tilesets, paths, backups, and TBX IDs | [Project Doctor](docs/PZ-Project-Doctor-Tiles-and-Paths.md) |
| WorldGen biomes, previews, features, and static prefabs | [WorldGen editor](docs/PZ-B42.20-WorldGen-Editor-and-Prefabs.md) |
| Jumbo, XL, and XXL trees | [Jumbo trees](docs/PZ-B42.20-Jumbo-Trees.md) |
| Build 42.20 Biomemap values and channels | [BiomeMapConfig](docs/PZ-B42.20-BiomeMapConfig.md) |
| Read-only adjacent PZW projects and `otherworld` compatibility | [Linked World Projects](docs/PZWorldEd-Linked-World-Projects.md) |
| Terrain images, Biomemap, Zombie Heatmap, InGameMap, and export | [PZTools user guide](docs/TileZed/PZToolsGuide.html) |

## TileZed

| Subject | Reference |
|---|---|
| Automapper concepts, manifest, layer rules, examples, interactive mode, and troubleshooting | [Automapper](docs/TileZed/Automapper.html) |
| Build 42 procedural loot viewer/editor shared with BuildingEd | [Procedural loot editor](docs/PZ-B42.20-Procedural-Loot-Editor.md) |
| Advanced `.pack` comparison, hashes, previews, extraction, and JSON provenance | [Pack comparator and extractor](docs/PZ-Pack-Comparator-and-Extractor.md) |
| `.tiles` comparison and Snow/Burnt/custom replacement editing | [TileDef comparator and Snow editor](docs/PZ-TileDef-Comparator-and-Snow-Editor.md) |
| TileDef property meanings | [Tile properties](docs/TileZed/TileProperties/index.html) |
| Terrain BMP tools inside TileZed | [BMP tools](docs/TileZed/BMPTools.html) |
| Lua batch/interactive mapping API and examples | [Lua scripting](docs/TileZed/LuaScripting.html) |
| RoomDef tools | [RoomDefecator](docs/TileZed/RoomDefecator.html) |
| Mapping-controlled spawn data | [Mapping spawn control](docs/TileZed/MappingSpawnControl.html) |

The complete Automapper example is under
`TileZed/examples/sewer_automap`.

## BuildingEd

The maintained manual lives under
`TileZed/src/tiled/BuildingEditor/manual` and is installed as
`docs/BuildingEd` in the portable release.

- [BuildingEd manual](docs/BuildingEd/index.html)
- [Lua scripting](docs/BuildingEd/LuaScripting.html)
- [Tools](docs/BuildingEd/Tools.html)
- [Tile mode](docs/BuildingEd/TileEditingMode.html)
- [Tiles dialog](docs/BuildingEd/TilesDialog.html)

The procedural-loot editor is shared with TileZed and uses the same loader,
project-output format, validation rules, and documentation.

## Documentation evidence labels

The maintained documents use these distinctions:

- **Game-confirmed** means the behavior or structure was checked against the
  local Project Zomboid 42.20 Lua/Java/data reference.
- **Tool-enforced** means it is an editor validation or safety policy and is
  not presented as an engine limit.
- **Representative preview** means the display is useful for authoring but is
  not claimed to reproduce a particular game save exactly.
- **Out of scope** identifies data that the current editor deliberately does
  not read, write, package, or simulate.

When one of these distinctions affects a workflow, the relevant document
states it next to the feature or limit.

## Documentation maintenance

User-visible behavior changes are not complete until all applicable locations
are updated:

1. the dedicated feature reference;
2. the user guide or documentation index;
3. `README.md`;
4. `RELEASE_CHANGELOG.md`;
5. `CHANGELOG-PZTOOLS.md`;
6. the copies installed in the portable distribution.

Before a release, internal links are checked and duplicate packaged/source
copies are compared by SHA-256.
