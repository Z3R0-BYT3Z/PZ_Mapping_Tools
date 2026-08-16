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

- **InGameMap > World Map Annotations Editor...** adds structured editing for
  Build 42.20 `worldmap-annotations.lua` text symbols. It supports translated
  and literal text, style, position, color, scale, anchor, rotation,
  perspective, zoom limits, duplication, removal, and atomic saving.
- **View > World Map Overlays** loads `worldmap.xml` and
  `worldmap-forest.xml` as independent read-only vector overlays. Geometry is
  cached in visible spatial batches to keep large maps responsive.
- Generate Building Features now recognizes RoomDefs embedded directly in cell
  TMX maps as well as separately placed TBX lots. Basement-only rooms are
  excluded and disconnected footprints remain separate.
- **Create World Image** now requests `MapToPNG.txt`, chooses the packaged file
  by default, and explains when terrain `Rules.txt` was selected by mistake.
- World-map XML and binary output is validated for the Build 42.20 signature,
  version, 256-square cell size, and dimensions before atomic replacement.
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

## TileZed

- Every BuildingEd entry point now launches the standalone `BuildingEd`
  executable. Tools actions, TBX opening, drag and drop, recent files, Lua
  requests, and building-check results no longer create an embedded editor
  owned by TileZed.
- Copy and cut immediately activate a translucent pointer-following preview for
  the complete selected layer and level scope. One click places it as a single
  Undo transaction.
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
