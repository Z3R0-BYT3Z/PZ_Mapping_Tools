# PZTools user-facing feature reference

This page is a routing table for the maintained PZWorldEd, TileZed, and
BuildingEd release. It covers major existing, restored, and newly added
functions. Detailed interaction instructions remain in the linked manuals.

## Shared suite behavior

| Function | Where | Reads | Writes / effect | Details |
|---|---|---|---|---|
| Initial setup | First editor startup | Extracted Tiles tree and packaged catalog directory | `settings/PZTools.ini` | Paths are shared by all three applications. |
| Change Shared Paths | WorldEd/TileZed preferences | Tiles parent and `config` | Shared portable settings | Restart other open editors after changing a shared path. |
| Audited configuration catalogues | Portable `config` | Build 42.20 data and configured Tiles tree | Shared definitions for all three applications | [File-by-file configuration reference](PZTools-Configuration-Files.md); project overrides stay outside the game and tool directories. |
| External themes | Preferences | `themes/*.qss` | Per-application theme choice | Theme changes widgets, not map rendering. |
| Complete tileset discovery | All three applications | Every valid PNG below 2x, then 1x/custom fallback | In-memory catalogue and startup log | 2x wins when both scales exist; 1x-only and 2x-only sheets remain valid. |
| Portable logs | Every run | Runtime messages | `settings/logs/<Application>-<timestamp>-<pid>.log` | Newest 20 files per application are retained. |
| Session/layout persistence | Every application | Portable application INI | Window, dock, splitter, recent-file, and selected-tool state | An unclean previous run skips automatic document restoration. |

See [Logs, diagnostics, and useful issue reports](Diagnostics-and-Logs.md) and
the [complete user guide](TileZed/PZToolsGuide.html).

### Common interaction model

New guided tools are designed so that a mapper can use them without first
learning the internal file formats:

1. the editor identifies the current project and states what it will inspect;
2. **Check** is read-only and produces a plain-language status;
3. one recommended action is presented when a safe correction exists;
4. changes are validated and backed up automatically;
5. file paths, ordered IDs, parser detail, and logs stay in an optional
   technical view for support.

An error should explain what is wrong, which file is involved, what remains
safe, and what the mapper can do next. Compatibility and file ownership are
shown in the workflow instead of being assumed knowledge.

## PZWorldEd

| Function | Menu or location | Main result |
|---|---|---|
| New 300 or 256 world | File > New World | PZW project with the selected native grid |
| Cell Copy and Paste | Edit menu or Ctrl+C and Ctrl+V | Complete cell data with a pointer-following preview, occupied-target warning, one placement per confirmation, and one Undo transaction |
| OpenStreetMap project | Terrain & Environment | New project with terrain, vegetation, roads, buildings, streets, and zones |
| Terrain and vegetation editor | Terrain & Environment | Atomic Map and vegetation PNG editing |
| BMP to TMX | Conversion & Export | Project-sized TMX cells with validation, repair, and metadata-only Rules/Blends synchronization |
| Project Doctor | Project Utilities | Read-only health report and backed-up project corrections |
| WorldGen biome editor | WorldGen | Project biome and feature Lua |
| WorldGen prefab editor | WorldGen | Static prefab Lua from supported sources |
| Street Names | Street Names dock | Version 1 `streets.xml` definitions |
| Regions | Regions dock | Independent rectangular `regions.lua` definitions |
| Biomemap Generator | Conversion & Export | Biome and zone channel images |
| Biomemap painting | Biomemap view | Separate red biome and green zone channel edits |
| Zombie Heatmap | ZombieMap view | Editable heatmap PNG with Undo and backup |
| Hole Detection | World tools | Missing level-zero tile detection and optional repair |
| Generate Lots | File menu | Legacy300 or one-to-one Native256 LOT output |
| Linked World Projects | World menu | Validated read-only PZW references aligned by World origin |
| InGameMap Forest | InGameMap menu | Forest XML, binary, PNG, and pyramid |
| Complete mod export | File menu | Map mod directory with generated map data |
| Basement entrance preview | Selected Basement zone | Translucent editable TBX or TMX entrance overlay |
| Integrated building basement | Placed lot context menu | Vertical lot placement, selected buried-level outlines, and confirmed ground opening at detected basement stairs |

Detailed references:

- [Project Doctor](PZ-Project-Doctor-Tiles-and-Paths.md)
- [WorldGen editor](PZ-B42.20-WorldGen-Editor-and-Prefabs.md)
- [Jumbo trees](PZ-B42.20-Jumbo-Trees.md)
- [BiomeMapConfig](PZ-B42.20-BiomeMapConfig.md)
- [Linked World Projects](PZWorldEd-Linked-World-Projects.md)

## TileZed

| Function | Menu or location | Main result |
|---|---|---|
| TMX editing | Main editor | Tile, object, layer, and RoomDef changes with Undo |
| Tileset palette | Tilesets dock | Complete catalogue with source and scale tags |
| Tile Definitions | Tools | Binary and text TileDef property editing |
| TileDef comparison | Tools | Structural comparison and controlled property merge |
| Snow and replacements | Tools | SnowTile, BurntTile, and custom replacement properties |
| Pack extraction | Tools | Individual tiles, tilesets, pages, or assembled objects |
| Pack comparison | Tools | Pixel, metadata, and file comparison |
| Depth Map Editor | Tools | Resizable geometry and depth-atlas editing with local and projected dimensions |
| Automapper | Automapping dock | Full or interactive rules applied through Undo |
| BMP Tools | Dockable BMP window | Terrain brush, fill, select, blending, and layer conversion |
| Select Tiles | Main tools | Layer-aware and floor-aware copy and cut, followed by a movable Ctrl+V placement preview |
| Procedural loot | Tools | Project loot manifest and generated post-merge Lua |
| Lua mapping | Tools and console | Transactional map, object, and RoomDef changes |

Feature references:

- [Automapper](TileZed/Automapper.html)
- [Procedural loot](PZ-B42.20-Procedural-Loot-Editor.md)
- [Pack comparator and extractor](TileZed/PZ-Pack-Comparator-and-Extractor.md)
- [TileDef comparator and Snow/Replacement editor](TileZed/PZ-TileDef-Comparator-and-Snow-Editor.md)
- [Lua scripting](TileZed/LuaScripting.html)
- [BMP tools](TileZed/BMPTools.html)

## BuildingEd

| Function | Menu / location | Availability | Main input | Main output or effect |
|---|---|---|---|---|
| Building/room editing | Main canvas and room tools | Building open/new | `.tbx` or new dimensions/template | Rooms, floors, walls, objects, and building metadata |
| Object tools | Main toolbar | Compatible editing mode | Walls, doors, windows, stairs, roofs, furniture | Semantic BuildingEd objects and generated tile layers |
| Tile mode | Tile mode and tileset dock | Complete Tiles catalogue loaded | Selected tileset/tile | Direct user-tile layer editing |
| Ortho/Iso category palettes | Object modes | Building catalogs loaded | `BuildingTiles.txt`, `BuildingFurniture.txt` | Category-driven object/furniture selection |
| Templates and furniture catalogs | Building dialogs | Catalog directory valid | `BuildingTemplates.txt`, `BuildingFurniture.txt` | Reusable building defaults and furniture groups |
| Basements/negative floors | Floor/building controls | Format supports levels | Building floor structure | Below-ground floors retained in TBX/TMX export |
| Autosave | Automatic every 2.5 minutes | Unsaved changes | Current building | `.autosave` beside TBX or under portable settings |
| Lua building automation | Building > Run Lua Script... / Lua Console | Building document open | Lua 5.2 editor script | One transactional Undo command or no change on failure |
| Procedural loot | Building > Procedural Loot Viewer / Editor... | Game Items path selected | Current RoomDef plus game/project loot data | Project JSON and generated post-merge Lua |
| Category validator | Command line | Deployed build and Tiles configured | Full catalog | Template, Tile, Furniture, Ortho/Iso, Lua and Undo PASS/FAIL log |

The installed [BuildingEd manual](BuildingEd/index.html) documents tile mode,
the Tiles dialog, drawing/object tools, and Lua scripting.

## File and directory ownership

| Data | Owner/source | Editor policy |
|---|---|---|
| Building and room editing | Main editor | TBX rooms, floors, walls, roofs, objects, and metadata |
| Tile mode | Tile mode | Direct user-tile layer editing |
| Select Tiles | Main tools | Layer-aware and floor-aware copy and cut, followed by a movable paste preview |
| Object palettes | Ortho and Iso modes | Category-driven walls, furniture, and objects |
| Templates | Building dialogs | Reusable building defaults and furniture groups |
| Basements | Floor controls | Negative floors retained in supported formats |
| Procedural loot | Building menu | Room-aware project loot overrides |
| Lua automation | Building menu and console | Transactional building changes with Undo |
| Autosave | Automatic | Recoverable copy of modified building data |

The [BuildingEd manual](BuildingEd/index.html) documents its editing
modes and tools.

## File ownership

| Data | Owner | Policy |
|---|---|---|
| Game Tiles, WorldGen, and loot | Project Zomboid installation | Read-only reference |
| Basement access TBX, TMX, and PZBY | Portable `pzby_tbx` beside `bin` | Read-only source and compiled access catalogue |
| PZW, TMX, TBX, and source PNG | Mapper project | Written only through explicit editor actions |
| Project WorldGen | Project `media/lua/server/WorldGen` | Atomic project output |
| Project loot | Project `media/lua/server/Items` | Project JSON and generated Lua |
| Catalogues | Portable `config` | Shared application data |
| Preferences and logs | Portable `settings` | Never used as the catalogue directory |
| Extracted pack data | User-selected directory | Safe rename unless overwrite is selected |

## Current limits

- WorldGen roads are not exposed by the current preview/editor.
- Erosion is planned as a separate possible module.
- WorldGen preview output is representative rather than save-identical.
- Static prefabs are z=0 four-slot schematics, not multi-floor buildings.
- Loot editing does not define items, icons, sandbox settings, vehicles, or
  complete mod packaging.
- Pack comparison/extraction does not merge or patch source packs.
- TileDef property merge does not invent missing tilesets or silently resize
  changed sheets.
- The internal DAY/NIGHT prototype is hidden because it does not yet match the
  current game lighting renderer closely enough for authoring claims.
