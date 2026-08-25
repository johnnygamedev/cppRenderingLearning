# 2D Physics Engine (C++ / OpenGL)
A custom 2D physics sandbox I built from scratch using C++ and OpenGL. Instead of relying on existing physics libraries, I wrote the solver, collision routines, and math setup myself to get a hands-on understanding of how engines handle constraints and rigid body dynamics under the hood.

# Key Features
Position-Based Dynamics (PBD): Wrote an iterative PBD solver for distance constraints, keeping connected bodies stable without sending them flying across the screen.

Rigid Body Motion: Custom integration step for 2D position, linear velocity, rotation, and angular velocity.

Collisions & Friction: Implemented collision checks between boxes and boundaries, complete with air resistance and floor friction so bodies actually settle instead of sliding forever.

Randomized Spawning: Set up C++ <random> distributions so pressing space drops new bodies from random spots with randomized speeds and spins.

OpenGL Render Loop: Custom GLSL shaders with uniform colors so it's easy to visually distinguish between free-falling objects and constrained bodies.

# Tech Used
Language: C++

Graphics: OpenGL, GLSL

Window & Input: GLFW

Math/RNG: Custom Vec2 struct, std::mt19937 RNG

# Controls & Controls Flow

Spacebar: Drops a new block with a random trajectory, position, and spin.

Constrained Pairs: Connected bodies spawn pre-calculated at their constraint distance so the solver doesn't snap violently on frame one.
