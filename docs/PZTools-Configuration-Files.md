# PZTools configuration files

This page explains the shared `config` directory used by PZWorldEd, TileZed,
and BuildingEd. It is written for mappers first: most users should not edit
these files by hand.

## The short version

- **Tiles** points to the extracted PNG tree.
- **Configuration** points to the PZTools `config` directory.
- **Settings** contains preferences and logs; it is not a configuration
  directory.
- Project files and mod overrides belong in the mapper's project, not in the
  game installation and not in PZTools `config`.

The three applications share the selected Tiles and configuration paths
through `settings/PZTools.ini`. Changing either path affects all three tools
after they are restarted.

## What each file does

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
| `MapToPNG.txt` | WorldEd Create World Image | Tile-to-color rules for rendering compiled LOT data to PNG |
| `Roads.txt` | WorldEd | Road presets |
| `Fences.txt` | TileZed | Fence presets and directional pieces |
| `Curbs.txt` | TileZed | Curb presets and directional pieces |
| `Edges.txt` | TileZed | Terrain-edge presets |
| `LuaTools.txt` | TileZed | Shipped Lua tool menu entries, icons, and scripts |
| `Rearrange.txt` | TileZed | Ordered tile selections for rearrangement tools |
| `RearrangeGrid.txt` | TileZed | Old and new tile-grid mappings |

WorldEd can synchronize the selected `Rules.txt` and `Blends.txt` into the
embedded metadata snapshot of existing assigned TMX maps without regenerating
their terrain images or rewriting their map content.

Legacy files such as `Textures.txt`, `TileShapes.txt`, and `thumbnails.txt`
are not active runtime catalogues.

## Current Build 42.20 audit

The August 4 audit checks the maintained catalogues against the configured
`C:\pz\Tiles` tree and the local Build 42.20 reference:

| Check | Audited result |
|---|---:|
| PNG files discovered recursively | 546 |
| Unique logical PNG base names | 543 |
| `Tilesets.txt` entries | 543 |
| Missing discovered names from the catalogue | 0 |
| Catalogue names without a readable PNG | 0 |
| Room names | 588 |
| Room tones/building types | 267 |
| TileDef property controls with tooltips | 211 |
| WorldEd profession choices | `all` plus 25 Build 42.20 professions |

Three logical names exist in more than one installed location. Selection is
deterministic: 2x wins over 1x/custom, and the maintained catalogue keeps the
chosen nested pack path. File names containing spaces are valid and are not
split into a different logical name.

Every tile reference in the maintained `Rules`, `Roads`, `Fences`, `Curbs`,
`Edges`, `BuildingTiles`, `BuildingTemplates`, `BuildingFurniture`,
`Rearrange`, `RearrangeGrid`, and explicit `Blends` entries was checked against
the logical sheet dimensions. Invalid burnt-roof names, six furniture groups
using nonexistent `signs_one-off_05_512` through `_559`, and two obsolete
rearrangement groups were removed or corrected.

Some special-effect PNGs do not encode a conventional 64x128 sheet rectangle.
When their decoded rectangle cannot represent their stored tile count, the
tools preserve the valid logical columns/rows recorded by the catalogue.
This is intentional for sheets such as the Giblet, Rain, and large-blood
effects; it is not permission to hide an arbitrary size mismatch.

## Safe customization

1. Back up the file you intend to change.
2. Keep project rules and WorldGen/loot overrides inside the project.
3. Never delete or reorder a TBX `tile_entry`, `user_tiles`, or furniture list
   by hand. Its position is an ID used elsewhere in the file.
4. Keep tile IDs within `columns x rows` from `Tilesets.txt`.
5. Restart all three applications after changing a shared catalogue.
6. Read the newest log under `settings/logs` if loading stops.

If a TMX or TBX project accumulated stale paths and definitions, use
**WorldEd > Tools > Project Doctor: Tiles and Paths...**. Its first
**Check project** pass is read-only, and the normal result is a plain-language
table. Technical parser details stay hidden unless support asks for them.

## Maintainer checks

Run validators from the deployed `bin` directory, where all Qt dependencies
are present:

```powershell
.\TileZed.exe --validate-tileset-catalog
.\BuildingEd.exe --validate-building-categories
.\PZWorldEd.exe --validate-world-defaults=..\config\WorldDefaults.txt
.\PZWorldEd.exe --validate-tileset-cleanup
```

`--rebuild-tileset-catalog=<Tiles-path>` is an explicit maintenance command.
It scans recursively, resolves 2x before 1x/custom, preserves valid logical
geometry where required, creates a backup, and rewrites the catalogue. It is
not part of ordinary application startup.
