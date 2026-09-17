#include "grid_map_2.h"

#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/mesh.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

// ============================================================
// Face directions
// ============================================================

static const Vector3i DIRECTIONS[6] = {
	Vector3i(0, 1, 0),	// TOP
	Vector3i(0, -1, 0),	// BOTTOM
	Vector3i(0, 0, 1),	// FRONT
	Vector3i(0, 0, -1),	// BACK
	Vector3i(1, 0, 0),	// RIGHT
	Vector3i(-1, 0, 0)	// LEFT
};

// ============================================================
// Face normals
// ============================================================

static const Vector3 NORMALS[6] = {
	Vector3(0, 1, 0),	// TOP
	Vector3(0, -1, 0),	// BOTTOM
	Vector3(0, 0, 1),	// FRONT
	Vector3(0, 0, -1),	// BACK
	Vector3(1, 0, 0),	// RIGHT
	Vector3(-1, 0, 0)	// LEFT
};

// ============================================================
// Binding
// ============================================================

void GridMap2::_bind_methods() {
	ClassDB::bind_method(
		D_METHOD("set_cell_size", "size"),
		&GridMap2::set_cell_size
	);

	ClassDB::bind_method(
		D_METHOD("get_cell_size"),
		&GridMap2::get_cell_size
	);

	ClassDB::add_property(
		"GridMap2",
		PropertyInfo(Variant::FLOAT, "cell_size"),
		"set_cell_size",
		"get_cell_size"
	);

	ClassDB::bind_method(
		D_METHOD("set_cell_item", "position", "item_id"),
		&GridMap2::set_cell_item
	);

	ClassDB::bind_method(
		D_METHOD("get_cell_item", "position"),
		&GridMap2::get_cell_item
	);

	ClassDB::bind_method(
		D_METHOD("clear"),
		&GridMap2::clear
	);

	ClassDB::bind_method(
		D_METHOD("rebuild_mesh"),
		&GridMap2::rebuild_mesh
	);

	ClassDB::bind_method(
		D_METHOD("create_test_platform"),
		&GridMap2::create_test_platform
	);
}

// ============================================================
// Constructor / Destructor
// ============================================================

GridMap2::GridMap2() {
	cell_size = 1.0f;
}

GridMap2::~GridMap2() {
}

// ============================================================
// Ready
// ============================================================

void GridMap2::_ready() {
	if (grid_cells.empty()) {
		create_test_platform();
	}
}

// ============================================================
// Cell Size
// ============================================================

void GridMap2::set_cell_size(float p_size) {
	if (p_size <= 0.0f) {
		return;
	}

	cell_size = p_size;
	rebuild_mesh();
}

float GridMap2::get_cell_size() const {
	return cell_size;
}

// ============================================================
// Cell Management
// ============================================================

void GridMap2::set_cell_item(const Vector3i &position, int item_id) {
	if (item_id < 0) {
		grid_cells.erase(position);
	} else {
		grid_cells[position] = item_id;
	}
}

int GridMap2::get_cell_item(const Vector3i &position) const {
	auto it = grid_cells.find(position);

	if (it != grid_cells.end()) {
		return it->second;
	}

	return -1;
}

void GridMap2::clear() {
	grid_cells.clear();
	rebuild_mesh();
}

// ============================================================
// Neighbor Check
// ============================================================

bool GridMap2::has_neighbor(
	const Vector3i &position,
	const Vector3i &direction
) const {
	const Vector3i neighbor_position = position + direction;

	return grid_cells.find(neighbor_position) != grid_cells.end();
}

// ============================================================
// Add Face
// ============================================================

void GridMap2::add_face(
	int face_idx,
	const Vector3 &pos,
	PackedVector3Array &vertices,
	PackedVector3Array &normals,
	PackedVector2Array &uvs,
	PackedInt32Array &indices
) {
	const int base_index = vertices.size();
	const float s = cell_size;

	Vector3 v[4];

	// --------------------------------------------------------
	// TOP (+Y)
	// --------------------------------------------------------

	switch (face_idx) {
		case 0:
			v[0] = pos + Vector3(0, s, s);
			v[1] = pos + Vector3(s, s, s);
			v[2] = pos + Vector3(s, s, 0);
			v[3] = pos + Vector3(0, s, 0);
			break;

		// ----------------------------------------------------
		// BOTTOM (-Y)
		// ----------------------------------------------------

		case 1:
			v[0] = pos + Vector3(0, 0, 0);
			v[1] = pos + Vector3(s, 0, 0);
			v[2] = pos + Vector3(s, 0, s);
			v[3] = pos + Vector3(0, 0, s);
			break;

		// ----------------------------------------------------
		// FRONT (+Z)
		// ----------------------------------------------------

		case 2:
			v[0] = pos + Vector3(0, 0, s);
			v[1] = pos + Vector3(s, 0, s);
			v[2] = pos + Vector3(s, s, s);
			v[3] = pos + Vector3(0, s, s);
			break;

		// ----------------------------------------------------
		// BACK (-Z)
		// ----------------------------------------------------

		case 3:
			v[0] = pos + Vector3(s, 0, 0);
			v[1] = pos + Vector3(0, 0, 0);
			v[2] = pos + Vector3(0, s, 0);
			v[3] = pos + Vector3(s, s, 0);
			break;

		// ----------------------------------------------------
		// RIGHT (+X)
		// ----------------------------------------------------

		case 4:
			v[0] = pos + Vector3(s, 0, s);
			v[1] = pos + Vector3(s, 0, 0);
			v[2] = pos + Vector3(s, s, 0);
			v[3] = pos + Vector3(s, s, s);
			break;

		// ----------------------------------------------------
		// LEFT (-X)
		// ----------------------------------------------------

		case 5:
			v[0] = pos + Vector3(0, 0, 0);
			v[1] = pos + Vector3(0, 0, s);
			v[2] = pos + Vector3(0, s, s);
			v[3] = pos + Vector3(0, s, 0);
			break;

		default:
			return;
	}

	// --------------------------------------------------------
	// Vertices + Normals
	// --------------------------------------------------------

	for (int i = 0; i < 4; i++) {
		vertices.push_back(v[i]);
		normals.push_back(NORMALS[face_idx]);
	}

	// --------------------------------------------------------
	// UV
	// --------------------------------------------------------

	uvs.push_back(Vector2(0, 1));
	uvs.push_back(Vector2(1, 1));
	uvs.push_back(Vector2(1, 0));
	uvs.push_back(Vector2(0, 0));

	// --------------------------------------------------------
	// Indices
	//
	// Reversed winding order so the outside of every face
	// is treated as the front face by Godot.
	// --------------------------------------------------------

	indices.push_back(base_index + 2);
	indices.push_back(base_index + 1);
	indices.push_back(base_index + 0);

	indices.push_back(base_index + 3);
	indices.push_back(base_index + 2);
	indices.push_back(base_index + 0);
}

// ============================================================
// Rebuild Mesh
// ============================================================

void GridMap2::rebuild_mesh() {
	// --------------------------------------------------------
	// Create MeshInstance3D if necessary
	// --------------------------------------------------------

	if (!mesh_instance) {
		mesh_instance = memnew(MeshInstance3D);

		mesh_instance->set_name("MeshDisplay");

		add_child(mesh_instance);
	}

	// --------------------------------------------------------
	// Empty grid
	// --------------------------------------------------------

	if (grid_cells.empty()) {
		mesh_instance->set_mesh(Ref<Mesh>());
		return;
	}

	// --------------------------------------------------------
	// Mesh buffers
	// --------------------------------------------------------

	PackedVector3Array vertices;
	PackedVector3Array normals;
	PackedVector2Array uvs;
	PackedInt32Array indices;

	// --------------------------------------------------------
	// Statistics
	// --------------------------------------------------------

	const int total_possible_faces = grid_cells.size() * 6;

	int generated_faces = 0;

	// --------------------------------------------------------
	// Generate visible faces
	// --------------------------------------------------------

	for (const auto &cell : grid_cells) {
		const Vector3i coord = cell.first;

		const Vector3 world_pos =
			Vector3(coord.x, coord.y, coord.z) * cell_size;

		for (int face = 0; face < 6; face++) {
			// Only generate faces that don't have a neighboring block.
			if (!has_neighbor(coord, DIRECTIONS[face])) {
				add_face(
					face,
					world_pos,
					vertices,
					normals,
					uvs,
					indices
				);

				generated_faces++;
			}
		}
	}

	// --------------------------------------------------------
	// Statistics
	// --------------------------------------------------------

	const int culled_faces =
		total_possible_faces - generated_faces;

	const int saved_percentage =
		total_possible_faces > 0
			? (culled_faces * 100) / total_possible_faces
			: 0;

	UtilityFunctions::print(
		"[GridMap2] Generated Faces: ",
		generated_faces,
		" | Culled Faces: ",
		culled_faces,
		" | GPU Load Saved: ",
		saved_percentage,
		"%"
	);

	// --------------------------------------------------------
	// Mesh Arrays
	// --------------------------------------------------------

	Array arrays;
	arrays.resize(Mesh::ARRAY_MAX);

	arrays[Mesh::ARRAY_VERTEX] = vertices;
	arrays[Mesh::ARRAY_NORMAL] = normals;
	arrays[Mesh::ARRAY_TEX_UV] = uvs;
	arrays[Mesh::ARRAY_INDEX] = indices;

	// --------------------------------------------------------
	// Create ArrayMesh
	// --------------------------------------------------------

	Ref<ArrayMesh> array_mesh;
	array_mesh.instantiate();

	array_mesh->add_surface_from_arrays(
		Mesh::PRIMITIVE_TRIANGLES,
		arrays
	);

	// --------------------------------------------------------
	// Apply Mesh
	// --------------------------------------------------------

	mesh_instance->set_mesh(array_mesh);
}

// ============================================================
// Test Platform
// ============================================================

void GridMap2::create_test_platform() {
	clear();

	for (int x = 0; x < 3; x++) {
		for (int z = 0; z < 3; z++) {
			set_cell_item(
				Vector3i(x, 0, z),
				1
			);
		}
	}

	rebuild_mesh();
}
