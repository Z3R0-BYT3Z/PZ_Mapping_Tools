# Project Doctor: TMX, TBX, tiles, and paths

Project Doctor is a guided WorldEd check for Project Zomboid Build 42 mapping
projects. It is designed for users who do not know the internal file formats
and for old projects assembled from several folders or tool versions.

Open it with **WorldEd > Tools > Project Doctor: Tiles and Paths...**.

The normal workflow has two steps:

1. **Check project** reads files and changes nothing.
2. **Fix safely...** shows a confirmation, creates a dated backup, and applies
   only the repairs classified as safe.

The large status and the four-column summary table are the result intended for
normal users. Each row says which file was checked, what the finding means,
and what the mapper can do next. Clean files are grouped into one reassuring
**Looks good** row instead of filling the screen.

The complete parser report is hidden behind **Show support details**. Open and
copy it only when support asks for it; seeing internal paths, counters, or
format names is not required to use Project Doctor.

## Which file belongs to which tool?

| File | Meaning | Normal editor |
|---|---|---|
| `.pzw` | World project, cell assignments, source/output paths | WorldEd |
| `.tmx` | One editable tile map/cell | TileZed |
| `.tbx` | Optional editable and reusable building source | BuildingEd |
| Tiles PNG | Sprite sheet used by all three tools | Configured shared Tiles tree |

“Build 42 compatible” does not mean that every file can be opened in every
application. It means that this release understands the maintained Build 42
mapping formats and data. Open TMX maps in TileZed, TBX buildings in
BuildingEd, and the PZW project in WorldEd.

A TMX map may be fully self-contained. Buildings can be painted or embedded
directly in its tile layers, object layers, and RoomDefs without retaining a
separate TBX source. Creating buildings in BuildingEd and placing TBX files is
a useful authoring convention, not a map-format requirement.

## Recommended project layout

Keep source files under one dedicated project folder:

```text
MyMapProject/
├── MyMap.pzw
├── images/
│   ├── map.png
│   └── map_veg.png
├── tmx/
│   ├── 0_0.tmx
│   └── tbx/
│       └── 0_0/
│           └── house.tbx
└── media/
    └── lua/
        └── server/
```

Do not edit a live project from Downloads, the Project Zomboid installation,
or a mixture of OneDrive and unrelated mod directories. Project Doctor reports
these locations because moving, syncing, updating, or deleting any one of them
can break otherwise valid relative paths.

## What the path check reports

When a PZW project is loaded, the check resolves its:

- assigned TMX maps;
- terrain source BMP;
- non-empty `rulesfile`, `blendsfile`, and `mapbasefile`;
- TMX export folder; and
- configured game media folder.

When a TMX contains object references whose `type` points to a TBX building,
Project Doctor inspects those optional source dependencies. A TBX reference is
classified as:

- **inside project and found** — safe to normalize;
- **missing** — preserved and reported;
- **outside project** — preserved and reported; or
- **absolute** — reported because moving the project will break it.

Project Doctor does not silently copy a building from Downloads or another
mod. It cannot know which copy the mapper intended to own. Move the correct
file into the project, update the map reference, and check again.

## Safe TMX cleanup rules

A TMX tileset declaration is not considered disposable merely because no tile
from it is currently painted.

PZ Mapping Tools deliberately retains the complete ordered catalogue header
used by legacy and adjacent maps. This keeps every valid sheet ready before
rendering and avoids changing behavior according to which map was opened
first. Project Doctor therefore uses these rules:

| Declaration state | Result |
|---|---|
| Valid PNG, used | Keep; normalize its image path |
| Valid PNG, currently unused | Keep for deterministic legacy compatibility; normalize its image path |
| Missing PNG, used by a tile/object/BMP rule | Keep and report as unresolved |
| Missing PNG, not used anywhere | Remove as a stale declaration |

BMP aliases, rules, blends, exclusion lists, placed tile layers, and tile
objects protect their referenced sheets. A missing used declaration is never
deleted merely to make the warning disappear.

Placed tile and tile-object references are counted directly from the raw TMX
GIDs. This remains reliable when the PNG is absent and the normal map loader
cannot construct the corresponding tile objects. Removing a declaration while
leaving those GIDs in place would shift their interpretation to another sheet,
so Project Doctor preserves the declaration by default.

For retained inline declarations, path resolution follows the same shared
catalogue policy as the editors:

1. readable 2x PNG;
2. readable 1x/custom PNG;
3. unresolved only when neither exists.

The stored image path becomes a portable relative path from the TMX to the
actual PNG selected by the tools. External TSX declarations can be retained or
removed when stale, but their internal image path is not rewritten by this
version.

## TBX safety

Project Doctor reports TBX dependencies and missing tilesets but does not
rewrite TBX files. Their internal `tile_entry`, `user_tiles`, and furniture
tables are semantic data. A stable second save is not sufficient proof that a
first canonical rewrite preserved the original building exactly.

Use BuildingEd for deliberate TBX edits.

## Backups and recovery

**Fix safely...** first copies every file that will change to:

```text
<project>/.pztools-backups/tileset-cleanup-YYYYMMDD-HHMMSS-mmm/
```

The project-relative folder structure is preserved. Each original is then
replaced atomically. Backup folders are excluded from later recursive checks,
so a second scan does not diagnose its own safety copies.

To undo a cleanup:

1. close WorldEd, TileZed, and BuildingEd;
2. find the latest dated cleanup folder;
3. copy the required original file back to the same relative project path;
4. reopen the project and run **Check project**.

If LOT files were generated after an incorrect cleanup, restore the source TMX
files first and regenerate the LOT output into a clean export directory. LOT
files do not repair themselves when the source is restored.

## Does cleanup make the tools faster?

Removing stale missing declarations and broken dependency paths avoids failed
lookups, confusing placeholders, and repeated user troubleshooting. It can
reduce work in project-specific loading paths.

It does not disable the intentional complete Tiles catalogue preload. Valid
unused TMX declarations remain available for compatibility, so Project Doctor
should not be sold as a universal startup-speed switch.

## If the status says “A few items need your help”

Resolve problems in this order:

1. unreadable or invalid PZW/TMX/TBX files;
2. assigned TMX or referenced TBX files that are missing;
3. source files outside the project or under Downloads/OneDrive;
4. used tilesets whose PNG cannot be resolved;
5. safe cleanup and path normalization.

Run **Check project** again after every move. Do not replace a missing file
with an unrelated file that happens to have the same name.
