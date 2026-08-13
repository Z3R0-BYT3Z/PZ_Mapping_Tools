# Changes since the BuildingEd room-floor hotfix

This release note starts after the August 10, 2026 BuildingEd hotfix that
preserved room floors across save, close, and reopen. That hotfix is the
baseline and is not repeated below.

## Shared Project Zomboid installation path

- WorldEd, TileZed, and BuildingEd Preferences now expose one shared,
  validated **Project Zomboid Installation** path.
- The selector accepts either the game root or its `media` directory and can
  detect standard Steam installations on first use.
- Existing explicit tool and project paths remain authoritative. When they
  are empty, the shared path supplies official TileDefs, texture packs,
  WorldGen biome and prefab definitions, and procedural-loot definitions.
- Clearing the field is remembered and does not trigger automatic detection
  again on the next start.
- The installed-game path remains separate from the mapping **Tiles** path.
  Renderable PNG sheets must still be extracted or supplied separately.

## Basement entrance placement preview

- Selecting a WorldEd Basement zone now resolves its `Access` name to a
  matching editable `ba_*.tbx` or `ba_*.tmx` source below the project, map,
  or configured Maps directory.
- WorldEd also searches the portable `pzby_tbx/basement_access` directory
  beside `bin` for editable sources and `pzby_tbx/binmap` for compiled PZBY
  files. Basement resources are not taken from the Project Zomboid
  installation.
- Both portable directories are searched recursively for TBX and TMX sources,
  including files copied there after WorldEd has started.
- TBX access sources are now loaded for the translucent preview instead of
  being detected but left unopened.
- Access sources do not require a staircase or an internal anchor. The complete
  TBX or TMX is displayed as a translucent overlay aligned directly to the
  Basement trace origin without centering or stair-coordinate offsets.
- The preview is visible only for the selected Basement zone, follows object
  movement, and never modifies the source access file.
- **Choose Basement Access...** opens a filterable list of portable access
  sources with dimensions and a large preview, then updates the selected
  Basement object's `Access` property through Undo.
- If only a compiled PZBY is available, WorldEd identifies it and
  explains that the editable TBX or TMX source is required for the preview.

## Integrated building basement placement

- A placed building lot can be lowered or raised by complete world levels
  from its WorldEd context menu. Its world X and Y remain fixed, so the lot
  visibly moves down or up in the isometric renderer.
- The placement level is stored in the PZW and is consumed by LOT generation.
  The source TBX or TMX is not rewritten.
- A lot remains selectable on every world level occupied by its source map.
  Its selected occupied volume is outlined in blue, with the portion placed
  below world level zero outlined in red.
- **Open Ground at Basement Stairs** detects staircase tops leading from
  level -1 to level 0 and removes the corresponding level-zero Floor tile
  from the affected cell TMX files after confirmation.
- Ground opening writes are atomic and create dated TMX backups beside the
  project before any source map is changed.

## Layer-aware and floor-aware tile selection

- TileZed and BuildingEd now share the same Select Tiles scope controls.
- Copy, cut, and delete can target the current layer, visible layers, all
  layers, or explicitly selected layers on the current level or every level.
- Clipboard data retains layer names and relative floor offsets.
- Multi-plane paste restores every copied plane relative to the selected
  destination anchor.

## Native256 LOT generation

- Generate Lots now waits for every assigned TMX and nested TBX source to
  finish loading before transferring the complete map composite to a worker.
  This prevents LOT headers from being written before building RoomDefs are
  available.
- Existing LOT output affected by missing RoomDefs must be regenerated from
  the original PZW, TMX, and TBX sources.
- RoomDef lookup bounds now follow the actual converted room rectangles,
  including negative coordinates and rectangles extending outside the nominal
  256-square source cell.
- Generate Lots includes optional on-the-fly hole filling. It can use the
  nearest available level-zero tile or an explicit tile name without changing
  source TMX and TBX files.
- Only holes that remain unresolved are included in the generation failure
  report.

## OpenStreetMap project generation

- Generated ground zones now use an exclusive priority classification for
  Water, TownZone, Farm, FarmLand, DeepForest, Forest, and remaining
  Vegitation squares.
- Specialized coverage is clipped per cell and merged into compact polygons
  or rectangles instead of producing thousands of overlapping zones.
- Major-road Nav uses width-aware polylines for motorway, trunk, primary, and
  secondary roads. Connected compatible sections are merged and clipped at
  cell boundaries.
- Building footprints remain covered by TownZone. Roads, railways, water,
  waterways, and bare sand are excluded from vegetation output.
- Railways, farmland, farmyards, orchards, vineyards, nurseries, allotments,
  forests, scrub, grass, tree rows, hedges, and individual trees receive
  dedicated terrain, vegetation, or zone handling where supported by OSM.
- A separate **Road markings** option creates editable WorldEd road objects
  for conservative supported two-way paved roads. Manual roads and edited
  generated roads are preserved during reimport.

## Pack extraction and multi-tile objects

- The Versatile Pack Extractor adds **All tiles**, **All tilesets**, and
  **All objects** presets that do not require a prefix or manually entered
  tileset name.
- Complete individual-tile extraction can create one automatic output
  subdirectory per tileset.
- Multi-tile furniture definitions can be assembled into one image per
  variant and orientation.
- Individual export can exclude tiles already used by assembled objects.
- Optional orphan-pixel correction removes very low alpha values and isolated
  visible pixels from reconstructed output without modifying source packs.
- Pack opening now indexes metadata without decoding every atlas page.
  Extraction decodes pages only when required and releases them after use.

## Compatibility corrections

- WorldEd, TileZed, and BuildingEd Preferences now provide autosave intervals
  of 1, 5, 10, 20, or 60 minutes plus Disabled. WorldEd and TileZed save only
  existing modified project files. BuildingEd retains recoverable `.autosave`
  copies and its restore workflow.
- InGameMap export now applies the renderer-complexity budget to XML and binary
  output. The conservative budget protects the game's signed 16-bit per-cell
  geometry offsets and simplifies highway polygons further before refusing an
  unsafe cell.
- Window Setup now applies **Use 1920 x 1080** immediately to the current
  application and provides **Apply to Current Application** for custom
  dimensions. Before either temporary action, the current size and position
  are preserved explicitly and return on the next normal start.
- Integrated the upstream LotPack null-map guard.
- Integrated applicable BuildingEd category filters while preserving the
  maintained complete tileset catalogue, current RoomNames, and renderer
  behavior.
- Biomemap raster handling now recognizes Water as green value 0.
- Project Doctor now counts raw TMX GIDs before deciding whether an unresolved
  tileset declaration is unused. Referenced declarations are preserved even
  when their PNG cannot currently be loaded, which prevents existing GIDs
  from being reinterpreted as tiles from another sheet.
- Project Doctor treats TBX files as read-only diagnostics. It reports their
  optional source dependencies but does not canonicalize or rewrite building
  data. Self-contained TMX maps do not require separate TBX files.
- Project Doctor logs every backup source, destination, and dated backup
  directory when fixes are applied.

## Credits and special thanks

- Tim Baker for the original WorldEd and TileZed work that remains the
  upstream foundation of these tools.
- A very special thank you to Fred 'Military Surplus' Cooper.
- Petro, Pabbiqo [pq], Dane, ! 𝕮𝖆ç𝖆𝖉𝖔𝖗, Kyber, шакалоблок, and the Project
  Zomboid mapping and modding community for reproducible reports, project
  files, screenshots, logs, and practical workflow feedback.

Legal authorship and third-party attribution remain documented in
`AUTHORS.txt`, `UPSTREAM-HISTORY.md`, and the bundled license notices.
