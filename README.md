# Computer-Graphics-3D-Classroom

A feature-rich, interactive **3D Classroom Scene** developed in C++ using **OpenGL 3.3 Core Profile** and **GLFW**. The project satisfies all core course requirements for 3D Computer Graphics: **hierarchical 3D modeling transformations**, **viewing transformations**, **multiple animated moving objects**, **multi-light Phong shading** (Point Lights and Spotlight), and rich material properties.

---

## 🌟 Features & Highlights

- **Modular Hierarchical Modeling**: All classroom elements (walls, tiled floor, student desks, chairs, teacher podium, laptop, blackboard, clock, ceiling fan, windows, door, and robot) are built hierarchically using unit cubes ($M = M_{\text{parent}} \times T \times R \times S$).
- **Viewing Transformations (4 Core Camera Perspectives)**:
  - `Key 1` / `B`: **Back View** (Initial View looking down the center aisle towards the blackboard)
  - `Key 2` / `G`: **Side View** (Angled profile view of student desks, windows, and ceiling fan)
  - `Key 3` / `T`: **Top View** (Bird's-eye overhead layout arrangement)
  - `Key 4` / `F`: **Teacher View** (View from behind the podium looking back at all student desks)
  - `Key 0` / `Home`: Instant reset to initial Back View
  - `Key V` / `Tab`: Sequentially cycle through all 4 views
- **Easy Camera Navigation**: Fully unified navigation using the **4 Arrow Keys**:
  - `Up / Down Arrow`: Move forward / backward
  - `Left / Right Arrow`: Smoothly turn and look left / right
  - `Shift + Up / Down Arrow`: Fly and elevate camera height up / down
  - `Shift + Left / Right Arrow`: Strafe sideways left / right
  - `Mouse Drag` & `Scroll`: Free look and zoom ($1.0^\circ$ to $45.0^\circ$ FOV)
- **Moving Objects & Dynamic Animations**:
  1. **Ceiling Fan**: Continuous smooth rotation around the vertical Y-axis (`SPACE` to toggle, `+`/`-` to adjust speed).
  2. **Wall Clock Second Hand**: Continuous **clockwise** rotation around the Z-axis calibrated to real-world speed ($6.0^\circ/\text{s}$, 1 full revolution in 60s).
  3. **Interactive Classroom Door**: Animated smooth opening and closing on its vertical hinge (`Key O`), featuring a **dynamic color transition** from deep walnut to illuminated warm golden honey-oak upon opening.
  4. **3D Classroom Robot**: Positioned beside the blackboard, featuring glowing electric-cyan visor eyes, chest status LEDs, and an articulated right arm that **continuously waves "bye-bye"** to the room!
- **Two Kinds of Lighting (Phong Shading)**:
  - **4 Point Lights**: Ceiling-mounted warm room fixtures with realistic quadratic distance attenuation ($k_c, k_l, k_q$).
  - **Spotlight**: Directional spotlight mounted on the ceiling pointing directly at the blackboard with smooth inner/outer cutoff cones ($18^\circ$ & $26^\circ$).
  - `Key L` (or `8`): Toggle point lights On / Off
  - `Key K` (or `9`): Toggle spotlight On / Off
  - `Keys 2–7`: Interactive fine-tuning of ambient, diffuse, and specular light intensities
- **Architectural Detailing**:
  - **Realistic Windows**: White hollow casings, protruding interior stone sill shelf, 6 divided glass panes with mullions, and an **outdoor campus scenery backdrop** (sunny blue sky, sunlight glow, lawn, and green trees).
  - **Modeled Hallway / Corridor**: Opening the door reveals a fully modeled school corridor outside with tiled flooring, far corridor wall, and an emissive warm ceiling light fixture, eliminating black voids.

---

## 🎮 Interactive Controls Reference

### 1. Camera Perspectives
| Key | Preset View | Description |
| :---: | :--- | :--- |
| **`1`** / **`B`** | View 1: Back View | Initial perspective looking down center aisle |
| **`2`** / **`G`** | View 2: Side View | Profile perspective of desks, fan, and windows |
| **`3`** / **`T`** | View 3: Top View | Overhead bird's-eye view of classroom arrangement |
| **`4`** / **`F`** | View 4: Teacher View | Front view from podium looking at students |
| **`0`** / **`Home`** | Reset View | Instant reset to View 1 |
| **`V`** / **`Tab`** | Cycle Views | Sequentially toggle: View 1 $\rightarrow$ 2 $\rightarrow$ 3 $\rightarrow$ 4 |

### 2. Camera Navigation
| Key | Action | Description |
| :---: | :--- | :--- |
| **`Up Arrow`** / `W` | Move Forward | Walk / move forward along view direction |
| **`Down Arrow`** / `S` | Move Backward | Walk / move backward |
| **`Left Arrow`** | Turn Left | Smoothly pan / turn camera to the left |
| **`Right Arrow`** | Turn Right | Smoothly pan / turn camera to the right |
| **`Shift + Up Arrow`** | Move Camera Up | Elevate camera height upward |
| **`Shift + Down Arrow`** | Move Camera Down | Lower camera height downward |
| **`Shift + Left / Right`** | Strafe Left / Right | Slide sideways left or right |
| `A` / `D` | Strafe Left / Right | Standard lateral movement |
| `Left / Right Mouse Drag` | Free Mouse Look | Rotate camera Pitch and Yaw smoothly |
| `Scroll Wheel` | Zoom | Adjust field of view ($1.0^\circ$ to $45.0^\circ$) |

### 3. Moving Objects & Animation
| Key | Action | Description |
| :---: | :--- | :--- |
| **`O`** | Open / Close Door | Smooth animated door swing with dynamic wood color transition |
| **Clock Second Hand** | Clockwise Sweep | Rotates continuously clockwise at real-world speed ($6^\circ/\text{s}$) |
| **3D Classroom Robot** | "Bye-Bye" Wave | Right arm continuously oscillates in a friendly waving gesture |
| **`SPACE`** | Toggle Ceiling Fan | Start / Stop fan rotation |
| **`+`** / **`=`** | Increase Fan Speed | Accelerate fan rotation by $+60^\circ/\text{s}$ |
| **`-`** / **`_`** | Decrease Fan Speed | Decelerate fan rotation by $-60^\circ/\text{s}$ |

### 4. Lighting Controls
| Key | Action | Description |
| :---: | :--- | :--- |
| **`L`** / **`8`** | Toggle Point Lights | Turn ceiling lamps On / Off |
| **`K`** / **`9`** | Toggle Spotlight | Turn blackboard spotlight On / Off |
| `2` / `3` | Ambient Light | Increase / Decrease ambient intensity |
| `4` / `5` | Diffuse Light | Increase / Decrease diffuse intensity |
| `6` / `7` | Specular Light | Increase / Decrease specular intensity |

### 5. Global 3D Model Transformations
| Key | Action | Description |
| :---: | :--- | :--- |
| `X`, `Y`, `Z` | Rotation Axis | Set room rotation axis to X, Y, or Z |
| `R` | Reverse Rotation | Rotate scene in reverse direction |
| `J` / `L` | Translate X | Move entire classroom left / right |
| `I` / `K` | Translate Y | Move entire classroom up / down |
| `P` | Translate Z | Move entire classroom backward |
| `C` / `N` | Scale X / Y | Expand / Compress along X / Y axis |
| `M` / `U` | Scale Z | Expand / Compress along Z axis |

---

## 🛠️ Build & Run Instructions

### Prerequisites
- Visual Studio 2022 / Visual Studio 2026 (MSVC v143 or v145)
- OpenGL 3.3 compatible graphics drivers
- GLFW3 and GLAD (configured in project settings)

### Building with MSBuild (Command Line)
```powershell
& "C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe" "Lighting.sln" /p:Configuration=Debug /p:Platform=x64
```

### Running the Application
```powershell
& "x64\Debug\Lighting.exe"
```

### Building in Visual Studio IDE
1. Open `Lighting.sln` in Visual Studio.
2. Select **Debug** configuration and **x64** platform.
3. Press **F5** (Local Windows Debugger) to compile and launch.
