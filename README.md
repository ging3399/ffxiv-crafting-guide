# ffxiv-crafting-guide
This is an automatic crafting rotation generator for ffxiv, written in C++.

## Introduction
Given your character status and recipe info, it'll automatically figure out an available(perhaps) rotation.
If it fails to find a proper rotation, there'll be some advice for you (only two pieces at present :D).

## User Guide
Open ffxiv-crafting-guide.exe, follow the instructions and you'll get the result.

## Limitations
1. There's no GUI yet.
2. The only languages supported are English and Simplified Chinese.
3. One can only let the rotation start with "Muscle Memory", "Reflect" and "Inner Quiet".
4. The character level is ignored and set to lvl80 at present. Manipulation is automatically learned.

## Updates
### version 0.0.3
Major update: now when the input character is not strong enough to reach 100% HQ on the recipe, one can choose to search for rotations that maximize quality.

### version 0.0.2
Fix the formulas of Basic + Standard Touch combo, and they won't be overused now.

### version 0.0.1
1. In-game macros will be generated now.
2. Fix a typo in Simplified Chinese language.

There are definitely a lot of bugs, and I sincerely welcome all kinds of issues and advice ;)
