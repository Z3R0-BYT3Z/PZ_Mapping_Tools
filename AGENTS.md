# PZ Mapping Tools source rules

## Repository provenance

Tim Baker created the original Project Zomboid WorldEd and TileZed foundation.
BuildingEd is historically part of the TileZed source tree. Alree / Unjammer
maintains and develops the unofficial Qt 5 continuation, its current features,
fixes, compatibility work, integrations, and combined releases, except where a
specific change has another recorded provenance.

Before changing authorship, credits, feature provenance, or statements about
who introduced a feature, read `FEATURE_PROVENANCE.md` and
`UPSTREAM-HISTORY.md`.

Do not erase earlier provenance merely because a later repository contains an
equivalent feature or fix. Compare dates and source. Preserve independent
parallel implementations and distinguish ideas or testing from implementation
authorship.

When porting a new Tim Baker upstream commit, update the `Tim Baker Upstream
Review` section of `FEATURE_PROVENANCE.md` and explicitly record whether the
commit was ported, adapted, already implemented, already fixed, not applicable,
or remains pending review.

Preserve copyright, license, third-party, generated-file, and public API
documentation notices. Do not add project-authored implementation comments.
Keep shipped documentation limited to implemented behavior and never include
private workflow material, internal paths, test narratives, or planned
features.
