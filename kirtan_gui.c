#include <stdio.h>
#include <string.h>
#include "raylib.h"
#include "food_structures.h"

// Pulling in functions written by Ali and Qasim from the backend layers
extern int load_nutritional_data(const char* filename, FoodItem dataset, int max_size);
extern void calculate_substitutes(FoodItem target, FoodItem datset, int size, FoodItems results );

// Global State Arrays for the UI Application
FoodItem app_dataset[200];
FoodItem display_results[3];
int total_available_foods=0;
int calculation_triggered=0;

// Hardcoded state representing a user dropdown selection box index
int active_dropdown_index =0;

/**
 * Initializes simulation data array bounds if Ali's CSV is not yet merged.
 * This keeps your GUI compileable and running smoothly on Week 1!
 */
void initialize_mock_gui_data(){
    strcpy(app_dataset[0].name, "White Rice");
    app_dataset[0].protein = 0.10; app_dataset[0].carbs = 0.90; app_dataset[0].fats = 0.02; app_dataset[0].calories = 0.65;

    strcpy(app_dataset[1].name, "Cauliflower Rice");
    app_dataset[1].protein = 0.08; app_dataset[1].carbs = 0.15; app_dataset[1].fats = 0.01; app_dataset[1].calories = 0.12;

    strcpy(app_dataset[2].name, "Avocado");
    app_dataset[2].protein = 0.09; app_dataset[2].carbs = 0.25; app_dataset[2].fats = 0.75; app_dataset[2].calories = 0.55;

    strcpy(app_dataset[3].name, "Sweet Potato");
    app_dataset[3].protein = 0.07; app_dataset[3].carbs = 0.60; app_dataset[3].fats = 0.01; app_dataset[3].calories = 0.40;

    total_available_foods = 4;
}

/**
 * Draw a mini custom bar chart metric block for a given food inside the UI frame.
 */
void DrawNutrientBar(int x, int y, const char* label, double value, color bar_color){
     DrawText(label, x, y, 16, DARKGRAY);
    // Render back drop empty container track
    DrawRectangle(x + 80, y + 2, 200, 14, LIGHTGRAY);
    // Render filled progress representation matching normalized double space (0.0 to 1.0)
    DrawRectangle(x + 80, y + 2, (int)(value * 200), 14, bar_color);
}
int main() {
    // 1. Initialize Window Framework Dimensions
    const int windowWidth = 850;
    const int windowHeight = 550;
    InitWindow(windowWidth, windowHeight, "Smart Diet Substitute & Nutrient Vector Engine");
    SetTargetFPS(60);

    // Load initial working dataset
    initialize_mock_gui_data();

    // 2. Core Interactive Presentation Application Window Loop
    while (!WindowShouldClose()) {
        
        // --- INTERACTION EVENT CONTROLLER PROCESSING ---
        Vector2 mousePos = GetMousePosition();
        
        // Simulating a button click collision check inside screen space rectangles
        Rectangle search_btn_bounds = { 50, 160, 220, 45 };
        if (CheckCollisionPointRec(mousePos, search_btn_bounds) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            // Target the selected food based on simulation index bounds
            FoodItem selected_target = app_dataset[active_dropdown_index];
            
            // EXECUTE QASIM'S CORE VECTOR CALCULATION MATCH ALGORITHM INSTANTLY!
            calculate_substitutes(selected_target, app_dataset, total_available_foods, display_results);
            calculation_triggered = 1;
        }

        // Simulating switching food dropdown options using Left/Right arrow keys
        if (IsKeyPressed(KEY_RIGHT)) {
            active_dropdown_index = (active_dropdown_index + 1) % total_available_foods;
            calculation_triggered = 0; // Clear display block states until recalculated
        }
        if (IsKeyPressed(KEY_LEFT)) {
            active_dropdown_index = (active_dropdown_index - 1 + total_available_foods) % total_available_foods;
            calculation_triggered = 0;
        }

        // --- GRAPHICS RENDERING RASTER LAYER ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Sidebar Control Frame Decoration
        DrawRectangle(0, 0, 320, windowHeight, LIGHTGRAY);
        DrawRectangle(10, 10, 300, windowHeight - 20, WHITE);
        DrawText("DIET CONTROL PANEL", 25, 30, 20, MAROON);
        
        // Render Active Input Target Profiles
        DrawText("Use Left/Right Arrows to Swap Target", 25, 75, 12, GRAY);
        DrawText("Active Food Target:", 25, 100, 15, DARKGRAY);
        DrawText(app_dataset[active_dropdown_index].name, 25, 125, 22, LIME);

        // Render Search Confirmation Switch Node
        DrawRectangleRec(search_btn_bounds, MAROON);
        DrawText("FIND SUBSTITUTES", 75, 172, 16, WHITE);

        // Render Application Workspace Information Block Header
        DrawText("VECTOR GRAPH METRICS DASHBOARD", 350, 30, 22, DARKGRAY);
        DrawLine(350, 60, 810, 60, LIGHTGRAY);

        if (calculation_triggered) {
            DrawText("Top 3 Mathematically Optimal Matches Found:", 350, 80, 15, ORANGE);
            
            int start_y = 120;
            for (int i = 0; i < 3; i++) {
                // Background visual card block for each result row
                DrawRectangle(350, start_y, 470, 120, LIGHTGRAY);
                DrawRectangle(352, start_y + 2, 466, 116, WHITE);
                
                // Display text names matching calculations
                char rank_label[100];
                sprintf(rank_label, "#%d Best Substitute: %s", i + 1, display_results[i].name);
                DrawText(rank_label, 370, start_y + 10, 18, BLACK);

                // Render Multi-dimensional Nutrient Bar Graphs onto the Display Panel Window
                DrawNutrientBar(370, start_y + 40, "Protein", display_results[i].protein, BLUE);
                DrawNutrientBar(370, start_y + 60, "Carbs", display_results[i].carbs, RED);
                DrawNutrientBar(370, start_y + 80, "Fats", display_results[i].fats, GOLD);
                
                start_y += 135; // Increment panel container index space boundaries
            }
        } else {
            DrawText("Select a food profile and click the trigger action\nto initialize 4D Euclidean distance array matching.", 400, 220, 16, GRAY);
        }

        EndDrawing();
    }

    // 3. De-allocate system allocations and close down process loops safely
    CloseWindow();
    return 0;
}







