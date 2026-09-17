#ifndef GRID_MAP_2_H
#define GRID_MAP_2_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/vector3i.hpp>
#include <unordered_map>

namespace godot {

class GridMap2 : public Node3D {
    GDCLASS(GridMap2, Node3D);

private:
    float cell_size;
    MeshInstance3D *mesh_instance = nullptr;

    struct Vector3iHash {
        std::size_t operator()(const Vector3i &v) const {
            return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1) ^ (std::hash<int>()(v.z) << 2);
        }
    };
    std::unordered_map<Vector3i, int, Vector3iHash> grid_cells;

    void add_face(int face_idx, const Vector3 &pos, PackedVector3Array &vertices, 
                  PackedVector3Array &normals, PackedVector2Array &uvs, PackedInt32Array &indices);

protected:
    static void _bind_methods();

public:
    GridMap2();
    ~GridMap2();

    void _ready() override;

    void set_cell_size(float p_size);
    float get_cell_size() const;

    void set_cell_item(const Vector3i &position, int item_id);
    int get_cell_item(const Vector3i &position) const;
    void clear();

    bool has_neighbor(const Vector3i &position, const Vector3i &direction) const;
    void rebuild_mesh();
    void create_test_platform(); // دالة اختبارية لبناء منصة 3x3 فوراً
};

} // namespace godot

#endif // GRID_MAP_2_H
