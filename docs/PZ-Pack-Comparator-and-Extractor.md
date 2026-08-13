# Project Zomboid `.pack` comparator and extractor

TileZed includes two complementary read-only inspection tools for Project
Zomboid texture packs. They support the legacy version-0 stream and the
current Build 42 `PZPK` version-1 stream.

Neither tool modifies the source `.pack`.

## Evidence and safety boundaries

Version-0 and `PZPK` version-1 headers, page metadata, texture rectangles,
trim offsets, and canvases are parsed by the same maintained reader used by
the tools and covered by deterministic self-tests. Pixel comparisons operate
on decoded, reconstructed RGBA canvases rather than encoded PNG bytes.

Dimension, count, allocation, path, and overwrite checks are
**tool-enforced** safety limits. The comparator and extractor do not claim to
patch, merge, or rewrite a source pack.

## Advanced comparator

Open **Tools > Advanced .pack Comparator...**, choose the baseline and
candidate packs, then select **Compare**.

The result table distinguishes:

- textures added to or removed from the candidate;
- decoded pixel changes;
- metadata changes;
- combined pixel and metadata changes;
- duplicate texture names in either pack;
- unchanged textures.

Search and status filters can be combined, and every column is sortable. The
summary identifies each pack's format, page and texture counts, and complete
file SHA-256. Selecting a result displays both reconstructed texture canvases,
a difference image, atlas-page location, packed rectangle, trim offset, canvas
size, alpha flag, and full hashes.

The difference preview uses:

- green for pixels added by the candidate;
- red for pixels removed from the candidate;
- magenta for changed pixels;
- grey for unchanged pixels.

Use **Copy report** or **Export CSV...** to preserve a stable comparison
report. The export includes full hashes rather than the shortened values used
in the table.

### Hash meanings

The hashes deliberately answer different questions:

- **Pack SHA-256** hashes the complete input file. It changes for any byte-level
  difference, including PNG encoding or atlas layout.
- **Pixel SHA-256** hashes the reconstructed RGBA texture canvas, including its
  dimensions. Moving an unchanged trimmed sprite within an atlas does not
  create a false pixel change.
- **Metadata SHA-256** hashes the page name, page alpha flag, packed rectangle,
  trim offset, and reconstructed canvas dimensions.

This separation makes atlas repacking visible as metadata without reporting
unchanged sprite art as modified pixels.

## Versatile extractor

Open **Tools > .pack Viewer / Extractor...**, load a pack, then choose
**Extract...**.

Opening a large pack indexes page and texture metadata without decoding every
atlas PNG. The extractor does not create thumbnails or calculate per-texture
pixel hashes before extraction. A page is decoded only when its pixels are
actually needed. Individual-tile and atlas-page exports release each page as
soon as its final selected source is written. Other modes release their pages
when the operation finishes. This keeps the extraction workflow responsive and
avoids retaining the complete decoded pack in memory.

The texture list includes the parsed tileset and tile number, source page,
reconstructed size, and packed rectangle. Checkboxes control the exact
extraction set. The visible rows can be selected or cleared in one action.

Three complete-export presets remove the need to type a prefix or tileset
name:

- **All tiles** clears the filter, selects the complete pack, prepares one
  reconstructed PNG per texture, and selects one output subdirectory per
  tileset automatically.
- **All tilesets** clears the filter, selects the complete pack, and prepares
  every tileset sheet whose texture names contain numeric tile IDs.
- **All objects** clears the filter, selects the complete pack, and prepares
  every complete multi-tile furniture object described by the active
  `BuildingFurniture.txt` catalogue.

The presets configure the selection and output mode. Choose the destination
directory, review the conflict policy, then press **Extract**.

Available filters are:

- contains;
- starts with;
- wildcard;
- regular expression;
- exact tileset name.

Simple filters can contain multiple semicolon-separated patterns. Matching is
case-insensitive by default and can be made case-sensitive.

### Output modes

- **Individual reconstructed textures** writes each selected texture on its
  complete transparent canvas, applying its trim offset.
- **One reconstructed sheet per tileset** parses names such as
  `floors_exterior_natural_01_42` and places tile 42 in the corresponding
  sheet cell.
- **Assembled multi-tile objects** uses the active `BuildingFurniture.txt`
  variant, orientation, and square coordinates to compose complete beds,
  sofas, tables, and other multi-square furniture as compact isometric PNGs.
  It writes one image per available variant and orientation under a directory
  named after the furniture group. Definitions with a missing selected tile
  are reported and skipped. A logical tile name that resolves to different
  pixel content in the same pack is treated as ambiguous rather than guessed.
- **Complete matching atlas pages** writes every source page referenced by the
  selection.

Reconstructed sheets support automatic geometry derived from the texture
canvases, explicit 1x cells (64 x 128), explicit 2x cells (128 x 256), or
custom cell dimensions. The sheet column count is configurable from 1 to 64.
Entries whose names cannot be parsed as a tileset plus numeric tile index are
reported and skipped only in sheet mode.

Outputs can be flat, grouped by source page, or grouped by tileset. Existing
files can be handled by safe renaming (the default), skipping, or explicit
overwrite. PNG files are committed atomically, and unsafe or Windows-reserved
filenames are normalized.

For individual extraction, **Exclude tiles that belong to multi-tile
objects** removes every texture referenced by a multi-square furniture
definition. This is intended for a two-pass workflow where complete objects
are exported first and the remaining standalone tiles are exported second.
Single-tile furniture is not excluded.

### Optional orphan-pixel correction

**Remove orphan pixels from reconstructed output** applies a deterministic
cleanup inspired by the useful TileSetZ cleanup behavior. It removes pixels
with alpha values from 1 through 4 and visible pixels that have no more than
two visible neighbors in their surrounding 3 x 3 area. Every decision is made
from the original image, so scan order cannot change the result.

The correction is optional and disabled by default because deliberate
one-pixel artwork can resemble an orphan. It applies to individual textures,
reconstructed tilesets, and assembled objects. Complete atlas pages remain
pixel-identical reconstructions and therefore do not use it. The source pack
is always read-only. The completion report and JSON manifest record the number
of removed pixels.

The optional version-3 JSON manifest records the source pack path and SHA-256,
pack version, selected texture metadata and metadata hashes, output mode,
object-tile exclusion settings and count, orphan-pixel settings and count,
assembled-object members and geometry, incomplete or ambiguous object counts,
and written outputs. Per-texture pixel hashes are deliberately not recomputed
for extraction because that would decode and copy every texture before useful
work starts. Pixel hashes remain available in the dedicated pack comparator.

## Input safety and limitations

The reader validates signatures, versions, counts, string lengths, encoded PNG
lengths, PNG headers and dimensions, packed rectangles, trim offsets, and
reconstructed canvas bounds while indexing. Full PNG decoding is deferred
until a viewer or exporter requests the page. PNG pages and reconstructed
sheets are subject to explicit dimension and allocation limits. Malformed or
truncated input is rejected with an explicit error. New packs and extracted
PNG or JSON files are committed atomically.

The comparator currently compares two packs at a time and exports CSV. It does
not create patches or merge packs. The extractor reconstructs PNG assets.
Object assembly depends on the active `BuildingFurniture.txt`, since a `.pack`
stores texture rectangles and names but does not store furniture
relationships. Use TileZed's existing pack creator when a new `.pack` must be
assembled.
