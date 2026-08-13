# PZTools configuration files

PZWorldEd, TileZed, and BuildingEd share the packaged `config` directory.
Most users should change these files only through the applications.

## Shared paths

- **Tiles** points to the extracted PNG tree used by the renderers.
- **Configuration** points to the packaged PZTools `config` directory.
- **Project Zomboid Installation** points to the game root or `media` folder.
- **Settings** contains preferences and logs. It is not configuration data.

The game installation is a read-only reference. Project rules and overrides
belong inside the mapper's project or mod.

## File ownership

| File | Used mainly by | Purpose |
|---|---|---|
| `Tilesets.txt` | All tools | Ordered logical sheet names, image paths, columns, and rows |
| `TileProperties.txt` | TileZed, BuildingEd | TileDef controls and tooltips |
| `BuildingTiles.txt` | BuildingEd | Walls, roofs, windows, grime, and object categories |
| `BuildingFurniture.txt` | BuildingEd, Pack Extractor | Furniture groups and ordered multi-tile definitions |
| `BuildingTemplates.txt` | BuildingEd | Building defaults and template categories |
| `TMXConfig.txt` | TileZed, BuildingEd | Layer and object layout for created maps |
| `RoomNames.txt` | BuildingEd | Room-name catalogue |
| `RoomTone.txt` | BuildingEd | Room-tone and building-type catalogue |
| `WorldDefaults.txt` | WorldEd | Object types, groups, properties, templates, colors, professions, and zone values |
| `Rules.txt` | WorldEd BMP tools | Color aliases and terrain-generation rules |
| `Blends.txt` | WorldEd BMP tools | Terrain edges and overlay transitions |
| `MapBaseXML.txt` | WorldEd BMP tools | Base TMX structure for generated maps |
| `Roads.txt` | WorldEd | Road presets |
| `Fences.txt` | TileZed | Fence presets and directional pieces |
| `Curbs.txt` | TileZed | Curb presets and directional pieces |
| `Edges.txt` | TileZed | Terrain-edge presets |
| `LuaTools.txt` | TileZed | Shipped Lua tool menu entries, icons, and scripts |
| `Rearrange.txt` | TileZed | Ordered tile selections for rearrangement tools |
| `RearrangeGrid.txt` | TileZed | Old and new tile-grid mappings |

Legacy files such as `MapToPNG.txt`, `Textures.txt`, `TileShapes.txt`, and
`thumbnails.txt` are not active runtime catalogues.

## Tileset resolution

The tools scan the configured Tiles tree recursively. A readable 2x sheet is
preferred. A readable 1x or custom sheet is used when no 2x sheet exists. A
placeholder appears only when no readable source resolves.

`Tilesets.txt` can store logical columns and rows for special-effect sheets
whose decoded PNG rectangle does not express their complete logical tile
count. This is required for supported effects and does not make arbitrary
image-size mismatches valid.

## Safe customization

1. Back up a catalogue before changing it.
2. Keep project Rules, Blends, WorldGen, and loot overrides in the project.
3. Do not reorder TBX `tile_entry`, `user_tiles`, or furniture lists by hand.
4. Keep tile IDs inside the declared columns and rows.
5. Restart the applications after changing shared catalogues.
6. Read the newest log if loading stops.

Project Doctor can inspect stale paths, missing tilesets, and TBX references.
Its first pass is read-only. Applied corrections create a backup.
