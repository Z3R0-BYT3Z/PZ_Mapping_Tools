# Linked World Projects in PZWorldEd

Linked World Projects expose the PZW `otherworld` system through the WorldEd
interface. A linked project is a read-only visual reference placed beside or
over the current project in the World view.

Use this when a large map is split into several PZW projects, when adjacent
areas are maintained separately, or when alignment must be checked without
merging their source files.

## Open the manager

Open a PZW project, then select **World > Linked World Projects...**.

The manager shows each linked project with:

- its project name and resolved path
- whether the PZW is ready, missing, duplicated, invalid, or incompatible
- its 256 x 256 or 300 x 300 cell grid
- its World origin
- its project size in cells
- its position relative to the current project
- its absolute world-cell coverage and any overlap with the current project

Use **Add Project...**, **Replace...**, **Remove**, **Move Up**, and
**Move Down** to maintain the list. **Refresh** rereads every PZW and updates
its status after an external change. **Apply** keeps the manager open while
updating the current project. **OK** applies the list and closes it.

Changes are part of the current PZW document. They support Undo and Redo and
are written by the normal Save and autosave workflows. WorldEd stores linked
paths relative to the current PZW whenever possible.

## Show or hide the references

Use **View > Show Linked World Projects** to show or hide every configured
reference. This is a display preference and does not remove links from the
PZW.

Linked cells, assigned maps, lots, and terrain images are rendered as context.
They cannot be selected or edited through the current project. They are not
merged into the current PZW and Generate Lots does not export their content.

## Placement

WorldEd does not store a separate manual offset for a linked project. It uses
the **World origin** from the Generate Lots settings of both projects.

```text
linked position = linked World origin - current World origin
```

For example, a current project at origin `10,20` and a linked project at
origin `14,18` place the linked project four cells to the right and two cells
above the current project.

To move a reference:

1. Open the linked PZW as a normal project.
2. Open its Generate Lots dialog.
3. Change its World origin.
4. Save the linked PZW.
5. Return to the manager in the current project and select **Refresh**.

Changing the current project's World origin also recalculates every linked
position immediately.

## Validation rules

The manager prevents:

- linking the current PZW to itself
- adding the same PZW more than once
- adding a missing or unreadable PZW
- mixing Native256 and Legacy300 projects

An overlap is allowed. It is reported as **Ready, overlaps** because overlapping
projects can be useful for alignment or staged migration.

Older PZW files may already contain broken `otherworld` paths. The manager
keeps those entries visible so they can be replaced or removed. It does not
silently discard them.

Only projects listed directly by the current PZW are displayed. Links inside
a linked PZW are not followed recursively.

## PZW representation

WorldEd maintains the XML automatically. The stored form remains compatible
with existing PZW files:

```xml
<otherworld path="../AdjacentMap/AdjacentMap.pzw"/>
```

Manual XML editing is no longer required.
