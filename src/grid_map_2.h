#ifndef GRID_MAP_2_H
#define GRID_MAP_2_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/vector3i.hpp>
#include <unordered_map>

namespace godot {

class GridMap2 : public Node3D {
    GDCLASS(GridMap2, Node3D);

private:
    float cell_size;
    // تخزين بيانات المكعبات: المفتاح هو الإحداثي (x, y, z)، والقيمة هي نوع البلوك (ID)
    // نستخدم Hash لسرعة الوصول في الذاكرة
    struct Vector3iHash {
        std::size_t operator()(const Vector3i &v) const {
            return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1) ^ (std::hash<int>()(v.z) << 2);
        }
    };
    std::unordered_map<Vector3i, int, Vector3iHash> grid_cells;

protected:
    static void _bind_methods();

public:
    GridMap2();
    ~GridMap2();

    void set_cell_size(float p_size);
    float get_cell_size() const;

    void set_cell_item(const Vector3i &position, int item_id);
    int get_cell_item(const Vector3i &position) const;

    // الدالة السحرية لحذف الأوجه المشتركة وفحص الجيران
    bool has_neighbor(const Vector3i &position, const Vector3i &direction) const;
    void rebuild_mesh();
};

} // namespace godot

#endif // GRID_MAP_2_H
