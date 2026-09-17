#include "grid_map_2.h"
#include <godot_cpp/classes/mesh.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

// الاتجاهات الستة للأوجه
static const Vector3i DIRECTIONS[6] = {
    Vector3i( 0,  1,  0), // 0: TOP (+Y)
    Vector3i( 0, -1,  0), // 1: BOTTOM (-Y)
    Vector3i( 0,  0,  1), // 2: FRONT (+Z)
    Vector3i( 0,  0, -1), // 3: BACK (-Z)
    Vector3i( 1,  0,  0), // 4: RIGHT (+X)
    Vector3i(-1,  0,  0)  // 5: LEFT (-X)
};

static const Vector3 NORMALS[6] = {
    Vector3( 0,  1,  0),
    Vector3( 0, -1,  0),
    Vector3( 0,  0,  1),
    Vector3( 0,  0, -1),
    Vector3( 1,  0,  0),
    Vector3(-1,  0,  0)
};

void GridMap2::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_cell_size", "size"), &GridMap2::set_cell_size);
    ClassDB::bind_method(D_METHOD("get_cell_size"), &GridMap2::get_cell_size);
    ClassDB::add_property("GridMap2", PropertyInfo(Variant::FLOAT, "cell_size"), "set_cell_size", "get_cell_size");

    ClassDB::bind_method(D_METHOD("set_cell_item", "position", "item_id"), &GridMap2::set_cell_item);
    ClassDB::bind_method(D_METHOD("get_cell_item", "position"), &GridMap2::get_cell_item);
    ClassDB::bind_method(D_METHOD("clear"), &GridMap2::clear);
    ClassDB::bind_method(D_METHOD("rebuild_mesh"), &GridMap2::rebuild_mesh);
    ClassDB::bind_method(D_METHOD("create_test_platform"), &GridMap2::create_test_platform);
}

GridMap2::GridMap2() {
    cell_size = 1.0f;
}

GridMap2::~GridMap2() {}

void GridMap2::_ready() {
    // إذا كانت الشبكة فارغة، ننشئ منصة 3x3 للتجربة البصرية فوراً
    if (grid_cells.empty()) {
        create_test_platform();
    }
}

void GridMap2::set_cell_size(float p_size) {
    cell_size = p_size;
    rebuild_mesh();
}

float GridMap2::get_cell_size() const {
    return cell_size;
}

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

bool GridMap2::has_neighbor(const Vector3i &position, const Vector3i &direction) const {
    Vector3i target = position + direction;
    return grid_cells.find(target) != grid_cells.end();
}

void GridMap2::add_face(int face_idx, const Vector3 &pos, PackedVector3Array &vertices, 
                       PackedVector3Array &normals, PackedVector2Array &uvs, PackedInt32Array &indices) {
    int base_index = vertices.size();
    float s = cell_size;
    Vector3 v[4];

    // حساب النقاط الأربعة للوجه بالترتيب الصحيح (CCW Winding)
    switch (face_idx) {
        case 0: // TOP (+Y)
            v[0] = pos + Vector3(0, s, s);
            v[1] = pos + Vector3(s, s, s);
            v[2] = pos + Vector3(s, s, 0);
            v[3] = pos + Vector3(0, s, 0);
            break;
        case 1: // BOTTOM (-Y)
            v[0] = pos + Vector3(0, 0, 0);
            v[1] = pos + Vector3(s, 0, 0);
            v[2] = pos + Vector3(s, 0, s);
            v[3] = pos + Vector3(0, 0, s);
            break;
        case 2: // FRONT (+Z)
            v[0] = pos + Vector3(0, 0, s);
            v[1] = pos + Vector3(s, 0, s);
            v[2] = pos + Vector3(s, s, s);
            v[3] = pos + Vector3(0, s, s);
            break;
        case 3: // BACK (-Z)
            v[0] = pos + Vector3(s, 0, 0);
            v[1] = pos + Vector3(0, 0, 0);
            v[2] = pos + Vector3(0, s, 0);
            v[3] = pos + Vector3(s, s, 0);
            break;
        case 4: // RIGHT (+X)
            v[0] = pos + Vector3(s, 0, s);
            v[1] = pos + Vector3(s, 0, 0);
            v[2] = pos + Vector3(s, s, 0);
            v[3] = pos + Vector3(s, s, s);
            break;
        case 5: // LEFT (-X)
            v[0] = pos + Vector3(0, 0, 0);
            v[1] = pos + Vector3(0, 0, s);
            v[2] = pos + Vector3(0, s, s);
            v[3] = pos + Vector3(0, s, 0);
            break;
    }

    for (int i = 0; i < 4; i++) {
        vertices.push_back(v[i]);
        normals.push_back(NORMALS[face_idx]);
    }

    uvs.push_back(Vector2(0, 1));
    uvs.push_back(Vector2(1, 1));
    uvs.push_back(Vector2(1, 0));
    uvs.push_back(Vector2(0, 0));

    // إضافة المثلثين اللذين يشكلان المربع
    indices.push_back(base_index + 0);
    indices.push_back(base_index + 1);
    indices.push_back(base_index + 2);
    indices.push_back(base_index + 0);
    indices.push_back(base_index + 2);
    indices.push_back(base_index + 3);
}

void GridMap2::rebuild_mesh() {
    if (!mesh_instance) {
        mesh_instance = memnew(MeshInstance3D);
        mesh_instance->set_name("MeshDisplay");
        add_child(mesh_instance);
    }

    if (grid_cells.empty()) {
        mesh_instance->set_mesh(Ref<Mesh>());
        return;
    }

    PackedVector3Array vertices;
    PackedVector3Array normals;
    PackedVector2Array uvs;
    PackedInt32Array indices;

    int total_possible_faces = grid_cells.size() * 6;
    int generated_faces = 0;

    // الخوارزمية: فحص كل بلوك والتأكد من جيرانه
    for (const auto &cell : grid_cells) {
        Vector3i coord = cell.first;
        Vector3 world_pos = Vector3(coord.x, coord.y, coord.z) * cell_size;

        for (int f = 0; f < 6; f++) {
            // إذا لم يكن هناك جار، يتم بناء الوجه؛ إذا كان هناك جار، يتم حذفه!
            if (!has_neighbor(coord, DIRECTIONS[f])) {
                add_face(f, world_pos, vertices, normals, uvs, indices);
                generated_faces++;
            }
        }
    }

    int culled_faces = total_possible_faces - generated_faces;
    UtilityFunctions::print("[GridMap2] Generated Faces: ", generated_faces, 
                            " | Culled (Hidden) Faces: ", culled_faces, 
                            " | Saved GPU Load: ", (culled_faces * 100) / total_possible_faces, "%");

    Array arrays;
    arrays.resize(Mesh::ARRAY_MAX);
    arrays[Mesh::ARRAY_VERTEX] = vertices;
    arrays[Mesh::ARRAY_NORMAL] = normals;
    arrays[Mesh::ARRAY_TEX_UV] = uvs;
    arrays[Mesh::ARRAY_INDEX] = indices;

    Ref<ArrayMesh> array_mesh;
    array_mesh.instantiate();
    array_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arrays);

    mesh_instance->set_mesh(array_mesh);
}

void GridMap2::create_test_platform() {
    clear();
    // إنشاء منصة 3x3 من البلوكات المتلاصقة
    for (int x = 0; x < 3; x++) {
        for (int z = 0; z < 3; z++) {
            set_cell_item(Vector3i(x, 0, z), 1);
        }
    }
    rebuild_mesh();
}
