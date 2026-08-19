# Changes since 2026-08-12

The exact public release baseline used for this changelog is
[`4fcc5eb3093c75adc4e584f37deb03afe0ad1f26`](https://github.com/Unjammer/PZ_Mapping_Tools/commit/4fcc5eb3093c75adc4e584f37deb03afe0ad1f26),
**Build 20260812**, committed on 2026-08-12 at 20:47 EDT. The similarly named
`42.20B260812` tag points to a sibling documentation commit based on the earlier
20260808 tree, so it is not the released source baseline used for this diff.

This changelog contains only user-visible work added after that exact source
state.

## Shared / All applications

- PZWorldEd, TileZed, and BuildingEd now expose their own **Help > About**
  action. Each dialog clearly identifies Tim Baker's original mapping-tools
  foundation, the Alree / Unjammer unofficial Qt 5 continuation, the canonical
  source URL, and the detailed feature and upstream provenance documents.
- Dark and colored themes now cover popup menus, combo lists, tooltips,
  disabled text, selections, editable fields, welcome content, tile previews,
  and application-drawn labels consistently.
- Built-in theme files are refreshed when packaged styles change while an
  explicitly selected custom preview background remains untouched.
- Fully transparent tiles from a resolved source remain valid in all three
  renderers and editors. **Show Invisible Tiles** uses a crossed-eye marker,
  while unresolved tile sources use a separate high-contrast diagnostic tile.
- The public documentation now separates original foundation, current
  continuation, feature-level provenance, later upstream ports, third-party
  components, ideas, testing, and special thanks.
- A unified troubleshooting FAQ now explains the visible warnings and errors
  from all three applications, the reason each message appears, the immediate
  corrective action, and how to prevent it from recurring.

## PZWorldEd

- `spawnpoints.lua` now writes complete absolute `posX`, `posY`, and `posZ`
  coordinates using the project cell size and configured World origin. It no
  longer emits the historical `worldX` and `worldY` fields tied to 300-square
  cells. SpawnPoint labels show the same absolute position in Cell View.
- New SpawnPoints default to the explicit `unemployed` profession. WaterFlow,
  WaterZone, and RoomTone creation applies the properties required by the
  Build 42 loader. Point-like WaterFlow and RoomTone records remain 1 x 1,
  while valid larger WaterZone areas are preserved.
- Lua export now omits malformed SpawnPoint, WaterFlow, WaterZone, and
  RoomTone records, explains every omitted record, and leaves the source PZW
  object available for correction. These warnings do not disable LOT
  generation.
- Main, vegetation, Biomemap, and project-bound OpenStreetMap workflows now
  require a confirmed saved PZW before opening. Untitled projects can no longer
  emit project images into a `bin/map` fallback directory.
- Moving selected cells now translates their Street points, Region anchors,
  and Road endpoints by the same cell offset. A terrain-image placement moves
  when its complete cell footprint is selected. These changes share the cell
  move's Undo transaction, while moving one cell intentionally moves only
  coordinates originating inside that cell.
- **World > Linked World Projects...** exposes the PZW `otherworld` system
  without manual XML editing. It supports add, replace, remove, ordering,
  refresh, Apply, Undo, and immediate scene reloading.
- The linked-project manager reports resolved path, grid format, World origin,
  project size, relative position, world-cell coverage, overlap, and clear
  validity states. It prevents self-links, duplicates, unreadable PZW files,
  and mixed Native256 or Legacy300 grids.
- **View > Show Linked World Projects** replaces the unclear Other Worlds
  label. Linked references remain read-only, direct-only visual context and
  are never merged into the current PZW or exported with its lots.
- Changing the current Generate Lots World origin now immediately recalculates
  linked-project placement and scene bounds.
- **InGameMap > World Map Annotations Editor...** adds structured editing for
  Build 42.20 `worldmap-annotations.lua` text symbols. It supports translated
  and literal text, style, position, color, scale, anchor, rotation,
  perspective, zoom limits, duplication, removal, and atomic saving.
- **View > World Map Overlays** loads `worldmap.xml` and
  `worldmap-forest.xml` as independent read-only vector overlays. Geometry is
  cached in visible spatial batches to keep large maps responsive. Overlay
  placement now uses absolute map-square coordinates, so Build 42.20
  256-square world-map data aligns over both Native256 and Legacy300 projects.
- Generate Building Features now recognizes RoomDefs embedded directly in cell
  TMX maps as well as separately placed TBX lots. Basement-only rooms are
  excluded and disconnected footprints remain separate.
- **Preferences > Feature Generation** now exposes four editable catalogues of
  exact tile names for Tree, Primary Road, Secondary Road, and Tertiary Road
  detection. Their defaults preserve the existing road classification and add
  classic trees, `jumbo_tree_01_0`, and the Build 42 Jumbo, XL, and XXL tree
  tiles. Add, remove, and restore-default controls are provided. An empty
  catalogue disables its detection, and all four catalogues persist in the
  portable WorldEd INI.
- **Create World Image** now requests `MapToPNG.txt`, chooses the packaged file
  by default, and explains when terrain `Rules.txt` was selected by mistake.
- World-map XML and binary output is normalized and validated for the Build
  42.20 signature, version, 256-square cell size, and dimensions before atomic
  replacement. XML now records `cellSize="256"`, and Legacy300 features are
  re-bucketed and clipped across the required 256-square world-map cells.
- BMP To TMX can synchronize only the embedded Rules and Blends metadata for
  all or selected cells. It previews changes, skips current files, creates
  dated backups, writes atomically, and leaves bitmap pixels, layers, objects,
  tilesets, masks, and edge settings unchanged.
- Native256 projects gain **Partial Chunks**, a dedicated menu and toolbar for
  selecting 8 x 8-square chunks on a 32 x 32 overlay. Click, drag, Ctrl+A,
  clear, saved `.tmx.pzchunks` masks, and the configured grid color are
  supported.
- Partial Chunk LOT generation exports tiles, rooms, buildings, objects,
  zombie intensity, and navigation only inside selected chunks. Omitted chunks
  are valid empty output, Hole Detection is disabled for the masked cell, and
  legacy 300-square projects retain complete-cell behavior.
- Point, polygon, and polyline tools use high-contrast icons that remain
  identifiable on light and dark toolbars.
- Copy checkpoints an existing modified PZW when autosave is enabled. Autosave
  remains suspended while cell and project clipboard macros are incomplete,
  and while progress-driven operations are applying partial changes.
- **View > Show Vehicle Mesh Previews** displays textured Project Zomboid
  vehicle meshes over ParkingStall and TrafficJam zones in Cell View. It reads
  text and binary FBX assets, vehicle scripts, zone distributions, and textures
  from the configured game installation without modifying project or game
  files. Navigation zones remain excluded. Vehicle controls now occupy their
  own **Preferences > Vehicles** tab. **Preview size** provides a persistent
  0.25 x to 4 x display correction, with
  1 x as the game-scale default, while retaining the zone anchor and
  orientation. The same group provides a separate persistent 0.25 x to 4 x
  render-quality multiplier.
  It increases or reduces the calculated image resolution without changing
  the vehicle's displayed dimensions, with an explicit memory and calculation
  time tradeoff and a 2.5 x default. Category names now select their real Lua
  distributions, including Police, emergency, Junkyard, TrafficJam, and
  ordinary parking categories. Default spawn density, normal, special, and
  burnt chances, weighted distribution aliases, `randomAngle`, `Direction`,
  `FaceDirection`, and TrafficJam angle variation produce a stable visual
  representation. The renderer now preserves each resource-model scale,
  vehicle-model scale, and vehicle-model offset instead of independently
  stretching the body axes to physics extents. It attaches the game wheel mesh
  using the vehicle model offset and each scripted wheel position, vertical
  offset, radius, and width, then aligns the body and wheel assembly to the
  ground together. Atlas quality automatically rises to the display correction
  when necessary, preventing a smaller cached sprite from being enlarged into
  a blurry preview. Named vehicle bodies are resolved correctly from FBX files
  containing multiple geometries. Existing preview-size settings are migrated
  to the new 1 x game-scale baseline.
  A persistent atlas under `settings/cache/vehicle-preview-atlas` is consulted
  before any mesh rasterization. It stores the baked body, texture, wheels,
  quality, and 16 possible directions, separated by a fingerprint of the game
  scripts, models, and textures. Preferences report its state and can rebuild
  all 16 directions for the installed vehicle catalogue. Existing
  eight-direction caches remain immediately usable through the nearest stored
  orientation until rebuilt. Atlas reconstruction now refreshes open cells and
  releases its temporary individual images. Cancellation preserves completed
  atlas entries.
- Vertical Placement now identifies the complete-lot operation, shows source
  and current world level ranges, previews the resulting range, and defaults
  confirmation to No. Moving lots between level groups in the Lots panel also
  requires confirmation. LOT generation logs every nonzero placement and
  stops before an edited project can export levels outside the supported range.

## TileZed

- The Layers and Levels visibility thresholds are saved to and restored from
  the portable application INI.
- The Depth Map Editor now uses movable splitters for the tileset, canvas,
  primitive list, and property controls. Their positions and the editor window
  geometry are restored between sessions, with **View > Reset Layout** for a
  clean default arrangement.
- Primitive dimensions now distinguish the Build 42 local geometry grid from
  the final isometric outline. The editor retains the compatible 64-pixel X/Z
  and 96-pixel Y increments, displays the actual projected bounding size, and
  explains the vertical `Z_SCALE` contribution.
- Every BuildingEd entry point now launches the standalone `BuildingEd`
  executable. Tools actions, TBX opening, drag and drop, recent files, Lua
  requests, and building-check results no longer create an embedded editor
  owned by TileZed.
- Copy captures the complete selected layer and level scope. Cut captures and
  removes it. Ctrl+V activates the translucent pointer-following preview, then
  one click places it as a single Undo transaction.
- Stamp and Fill previews retain explicit tileset ownership. Undoing the TMX
  addition of a clipboard-only tileset can no longer leave the active preview
  with invalid tile pointers.
- Copy checkpoints an existing modified TMX when autosave is enabled. Cut,
  paste, tile painting, BMP paste, and map resize keep autosave suspended until
  their complete Undo transaction is available.
- Partial Chunks shares the Native256 selection, persistence, toolbar, and LOT
  export workflow described above. Normal tile Select All remains unchanged
  while the mode is disabled.
- BMP Rules and Blends regeneration now uses resolved tileset and candidate
  indexes, sparse dirty regions, scan-line pixel copies, compact brush runs,
  and patch-only MiniMap updates.
- Ground erase avoids repeated metadata parsing and whole-map warning scans.
  Disconnected brush changes no longer expand into one large rectangular
  recalculation.
- Exact rule tiles take priority over similarly named custom or test sheets,
  preventing a name such as `blends_natural_01_TEST` from being interpreted as
  a numeric Sand tile reference.
- Tileset removal safely pauses the MiniMap worker, invalidates Rules and
  Blends caches, rejects stale indexes, restores matching state through Undo,
  and distinguishes stored map content from metadata-only references.
- Tileset dock commands remain reachable through Qt's toolbar overflow when a
  dock is narrow.
- Formerly black brush-shape, fence, and related artwork uses a high-contrast
  double-outline design for light and dark themes.

## BuildingEd

- The Layers visibility threshold is saved to and restored from the portable
  application INI.
- Copy and cut retain cloned RoomDefs, room layouts, tiles, layer names, and
  relative floor offsets across every selected plane. Pasted rooms appear
  immediately in Ortho view.
- Paste no longer requires a destination selection. The complete clipboard
  follows the pointer as a translucent object, left-click places it, and
  right-click cancels it.
- Placement beyond any edge expands the building automatically. Existing room
  grids, user tiles, properties, objects, selections, and basement access are
  translated when top or left expansion changes the origin.
- Room creation, tile placement, room placement, expansion, and translated
  content are grouped into one Undo and Redo transaction.
- Building-owned RoomDefs are created before pasted room grids are assigned,
  preventing the clipboard from leaving dangling room references.
- Clipboard preview now falls back to an available layer when the requested
  target layer is absent and rejects out-of-range renderer-vector access. This
  fixes the intermittent copy and paste access violation reported on August 15.
- Moving a clipboard preview now reuses its converted isometric tile grid
  instead of resolving every copied tile again for each pointer position.
- Room name and internal-name edits now commit when editing finishes or a list
  value is selected. Metadata-only room changes no longer rebuild every
  isometric floor.
- Auto-expansion copies every floor from its actual stored grid dimensions
  before the building size changes. Expansion can no longer read beyond the
  previous room grid or leave invalid RoomDef references behind.
- Replacing an occupied tile during paste preserves the tile-grid occupancy
  count instead of incrementing it again.
- Copy checkpoints the recoverable autosave when unsaved edits exist. Timed
  autosave remains suspended during complete cut and paste Undo transactions
  and resumes only after their final coherent state is available.
- The New Building dialog sizes itself from its themed contents so fields and
  buttons are not clipped by display scaling or dark themes.

## Fixes

- InGameMap road generation no longer treats generic dirt tiles as trails by
  default. Trail inference is an explicit preference, water-covered squares
  are excluded, and regenerating roads removes previously generated trail
  features when the option is disabled.
- Generated primary, secondary, tertiary, Trail, and railway masks now close
  isolated one-tile breaks, fill small enclosed holes, and discard short
  internal fragments before polygon conversion. Long narrow paths and valid
  cell-edge continuations remain preserved. Tile properties identifying water
  also prevent dirt underneath water overlays from becoming Trails.
- Restored TileZed's copy and paste workflow. Ctrl+C only captures the
  selection, Cut captures and removes it, and Ctrl+V opens the movable
  placement preview.
- Fixed the Depth Map Editor presenting local Y geometry steps as though they
  were the complete projected height. The displayed outline now accounts for
  vertical projection and the X/Z isometric footprint.
- Fixed a BuildingEd clipboard-preview memory corruption. A copied selection
  containing user layers could request a layer not present in the preview
  composite, produce index `-1`, write before the renderer's parallel vectors,
  and later crash inside `QVector<QGraphicsItem *>::operator[]`.
- Fixed BuildingEd room-grid corruption when a paste expanded the building.
  Floor resizing previously used the already-expanded global dimensions while
  reading the old floor grid.
- Check Maps now distinguishes an unresolved tile source from a valid
  transparent tile and reports the missing recommended `invisible` TileDef
  property separately.
- Fixed dark-theme controls that retained white canvases, black text, or
  unreadable selected entries in BuildingEd and tileset dialogs.
- Fixed inaccessible TileZed Tilesets actions when the dock was collapsed too
  narrowly.
- Fixed a TileZed clipboard lifetime fault where Undo could delete a tileset
  still referenced by the pointer-following Stamp or Fill preview.
- Prevented WorldEd and TileZed autosave from serializing an intermediate state
  while a clipboard, painting, resize, or progress-driven operation is still
  applying its Undo macro.

## Credits and special thanks

- Tim Baker for the original WorldEd and TileZed work that remains the upstream
  foundation of these tools.
- Alree / Unjammer for the unofficial Qt 5 continuation, current maintenance,
  new features, fixes, integrations, and releases.
- A very special thank you to Fred 'Military Surplus' Cooper.
- Petro, Pabbiqo [pq], Dane, ! Cacador, Kyber, shakaloblok, and the Project
  Zomboid mapping and modding community for reproducible reports, project
  files, screenshots, logs, and practical workflow feedback.

Legal authorship and third-party attribution are documented in `AUTHORS.txt`,
`FEATURE_PROVENANCE.md`, `UPSTREAM-HISTORY.md`, and the bundled license notices.
