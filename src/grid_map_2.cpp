#include "grid_map_2.h"
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void GridMap2::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_cell_size", "size"), &GridMap2::set_cell_size);
    ClassDB::bind_method(D_METHOD("get_cell_size"), &GridMap2::get_cell_size);
    ClassDB::add_property("GridMap2", PropertyInfo(Variant::FLOAT, "cell_size"), "set_cell_size", "get_cell_size");

    ClassDB::bind_method(D_METHOD("set_cell_item", "position", "item_id"), &GridMap2::set_cell_item);
    ClassDB::bind_method(D_METHOD("get_cell_item", "position"), &GridMap2::get_cell_item);
    ClassDB::bind_method(D_METHOD("rebuild_mesh"), &GridMap2::rebuild_mesh);
}

GridMap2::GridMap2() {
    cell_size = 1.0f;
}

GridMap2::~GridMap2() {}

void GridMap2::set_cell_size(float p_size) {
    cell_size = p_size;
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
    return -1; // فارغ
}

bool GridMap2::has_neighbor(const Vector3i &position, const Vector3i &direction) const {
    Vector3i target = position + direction;
    return grid_cells.find(target) != grid_cells.end();
}

void GridMap2::rebuild_mesh() {
    UtilityFunctions::print("[GridMap2] Rebuilding mesh with internal face culling...");
    // هنا سنضع كود الـ ArrayMesh وتوليد المضلعات فقط للأوجه غير الملتصقة
}
