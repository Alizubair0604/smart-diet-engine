#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "raylib.h"
#include "food_structures.h"

#define MAX_FOODS    50000
#define VISIBLE_ROWS 7

// Global state for the UI application
static FoodItem app_dataset[MAX_FOODS];
FoodItem display_results[3];
int total_available_foods = 0;
int calculation_triggered = 0;

// Search state
static char search_text[64] = "";
static int  match_list[MAX_FOODS];   // indices of foods that match the search text
static int  match_count = 0;
static int  list_scroll = 0;         // first visible row of the match list
static int  selected_index = -1;     // index into app_dataset of the chosen food

// Case-insensitive "does text contain query?"
static int contains_ci(const char *text, const char *query) {
    if (query[0] == '\0') return 1;
    for (int i = 0; text[i]; i++) {
        int j = 0;
        while (query[j] && text[i + j] &&
               tolower((unsigned char)text[i + j]) == tolower((unsigned char)query[j])) j++;
        if (query[j] == '\0') return 1;
    }
    return 0;
}

static void rebuild_matches(void) {
    match_count = 0;
    for (int i = 0; i < total_available_foods; i++)
        if (contains_ci(app_dataset[i].name, search_text))
            match_list[match_count++] = i;
    list_scroll = 0;
}

// Draws text, shortening it with "..." so it never exceeds max_width pixels
static void DrawTextClipped(const char *text, int x, int y, int size, Color color, int max_width) {
    char buf[128];
    strncpy(buf, text, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';
    int len = (int)strlen(buf);
    if (MeasureText(buf, size) > max_width) {
        while (len > 3 && MeasureText(buf, size) > max_width) {
            len--;
            buf[len] = '\0';
            if (len >= 3) { buf[len - 1] = '.'; if (len >= 2) buf[len - 2] = '.'; if (len >= 3) buf[len - 3] = '.'; }
        }
    }
    DrawText(buf, x, y, size, color);
}

// Mini bar chart: value is normalized 0.0 - 1.0, 'real_text' is the real-unit label
static void DrawNutrientBar(int x, int y, const char *label, double value, const char *real_text, Color bar_color) {
    if (value < 0.0) value = 0.0;
    if (value > 1.0) value = 1.0;
    DrawText(label, x, y, 16, DARKGRAY);
    DrawRectangle(x + 80, y + 2, 200, 14, LIGHTGRAY);
    DrawRectangle(x + 80, y + 2, (int)(value * 200), 14, bar_color);
    DrawText(real_text, x + 290, y, 14, DARKGRAY);
}

int main() {
    const int windowWidth = 850;
    const int windowHeight = 550;
    InitWindow(windowWidth, windowHeight, "Smart Diet Substitute & Nutrient Vector Engine");
    SetTargetFPS(60);

    // Load the REAL dataset (Ali's loader + Qasim's engine do the rest)
    total_available_foods = load_nutritional_data("nutrition_dataset.csv", app_dataset, MAX_FOODS);
    int load_failed = (total_available_foods <= 0);
    if (!load_failed) {
        rebuild_matches();
        selected_index = match_list[0];
    }

    Rectangle search_box = { 25, 100, 270, 30 };
    Rectangle search_btn_bounds = { 25, 430, 270, 45 };

    while (!WindowShouldClose()) {

        // --- INPUT ---
        Vector2 mousePos = GetMousePosition();
        int changed = 0;

        // Typing into the search box
        int ch = GetCharPressed();
        while (ch > 0) {
            int len = (int)strlen(search_text);
            if (ch >= 32 && ch <= 126 && len < (int)sizeof(search_text) - 1) {
                search_text[len] = (char)ch;
                search_text[len + 1] = '\0';
                changed = 1;
            }
            ch = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
            int len = (int)strlen(search_text);
            if (len > 0) { search_text[len - 1] = '\0'; changed = 1; }
        }
        if (changed && !load_failed) rebuild_matches();

        // Scroll the match list with the mouse wheel or Up/Down keys
        float wheel = GetMouseWheelMove();
        if (wheel < 0 || IsKeyPressed(KEY_DOWN) || IsKeyPressedRepeat(KEY_DOWN)) list_scroll++;
        if (wheel > 0 || IsKeyPressed(KEY_UP)   || IsKeyPressedRepeat(KEY_UP))   list_scroll--;
        if (list_scroll > match_count - VISIBLE_ROWS) list_scroll = match_count - VISIBLE_ROWS;
        if (list_scroll < 0) list_scroll = 0;

        // Clicking a row selects that food
        for (int r = 0; r < VISIBLE_ROWS && list_scroll + r < match_count; r++) {
            Rectangle row = { 25, (float)(140 + r * 28), 270, 26 };
            if (CheckCollisionPointRec(mousePos, row) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                selected_index = match_list[list_scroll + r];
                calculation_triggered = 0;
            }
        }

        // Find-substitutes button (or Enter key)
        if (!load_failed && selected_index >= 0 &&
            ((CheckCollisionPointRec(mousePos, search_btn_bounds) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) ||
             IsKeyPressed(KEY_ENTER))) {
            calculate_substitutes(app_dataset[selected_index], app_dataset, total_available_foods, display_results);
            calculation_triggered = 1;
        }

        // --- DRAWING ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRectangle(0, 0, 320, windowHeight, LIGHTGRAY);
        DrawRectangle(10, 10, 300, windowHeight - 20, WHITE);
        DrawText("DIET CONTROL PANEL", 25, 25, 20, MAROON);

        if (load_failed) {
            DrawText("Could not load nutrition_dataset.csv", 25, 100, 14, RED);
            DrawText("Put it in the same folder as the .exe", 25, 120, 14, GRAY);
        } else {
            char info[96];
            snprintf(info, sizeof(info), "%d foods loaded", total_available_foods);
            DrawText(info, 25, 55, 14, GRAY);
            DrawText("Type a food name to search:", 25, 80, 14, DARKGRAY);

            DrawRectangleRec(search_box, RAYWHITE);
            DrawRectangleLines((int)search_box.x, (int)search_box.y, (int)search_box.width, (int)search_box.height, MAROON);
            DrawTextClipped(search_text, 32, 106, 18, BLACK, 250);
            if (((int)(GetTime() * 2)) % 2 == 0) {
                int cx = 32 + MeasureText(search_text, 18);
                if (cx > 285) cx = 285;
                DrawRectangle(cx + 1, 105, 2, 20, MAROON);
            }

            snprintf(info, sizeof(info), "%d matches (scroll / Up-Down)", match_count);
            DrawText(info, 25, 125 - 0, 10, GRAY);

            for (int r = 0; r < VISIBLE_ROWS && list_scroll + r < match_count; r++) {
                int idx = match_list[list_scroll + r];
                Rectangle row = { 25, (float)(140 + r * 28), 270, 26 };
                Color bg = (idx == selected_index) ? (Color){ 255, 220, 220, 255 } : RAYWHITE;
                if (CheckCollisionPointRec(mousePos, row)) bg = (Color){ 235, 235, 235, 255 };
                DrawRectangleRec(row, bg);
                DrawRectangleLines((int)row.x, (int)row.y, (int)row.width, (int)row.height, LIGHTGRAY);
                DrawTextClipped(app_dataset[idx].name, 30, 146 + r * 28, 14, DARKGRAY, 260);
            }

            DrawText("Selected food:", 25, 345, 14, DARKGRAY);
            if (selected_index >= 0) {
                FoodItem *s = &app_dataset[selected_index];
                DrawTextClipped(s->name, 25, 365, 16, MAROON, 275);
                char vals[128];
                snprintf(vals, sizeof(vals), "P %.1fg  C %.1fg  F %.1fg  %.0f kcal",
                         s->protein * PROTEIN_SCALE, s->carbs * CARBS_SCALE,
                         s->fats * FATS_SCALE, s->calories * CALORIES_SCALE);
                DrawText(vals, 25, 390, 13, GRAY);
                DrawText("(per 100 g)", 25, 408, 11, GRAY);
            }

            DrawRectangleRec(search_btn_bounds, MAROON);
            DrawText("FIND SUBSTITUTES", 75, 442, 16, WHITE);
        }

        // Results dashboard
        DrawText("VECTOR GRAPH METRICS DASHBOARD", 350, 30, 22, DARKGRAY);
        DrawLine(350, 60, 810, 60, LIGHTGRAY);

        if (calculation_triggered) {
            DrawText("Top 3 Mathematically Optimal Matches Found:", 350, 80, 15, ORANGE);

            int start_y = 105;
            for (int i = 0; i < 3; i++) {
                if (display_results[i].name[0] == '\0') continue;
                FoodItem *f = &display_results[i];

                DrawRectangle(350, start_y, 470, 130, LIGHTGRAY);
                DrawRectangle(352, start_y + 2, 466, 126, WHITE);

                char rank_label[140];
                snprintf(rank_label, sizeof(rank_label), "#%d  %s", i + 1, f->name);
                DrawTextClipped(rank_label, 365, start_y + 8, 17, BLACK, 440);

                char t[32];
                snprintf(t, sizeof(t), "%.1f g", f->protein * PROTEIN_SCALE);
                DrawNutrientBar(365, start_y + 36, "Protein", f->protein, t, BLUE);
                snprintf(t, sizeof(t), "%.1f g", f->carbs * CARBS_SCALE);
                DrawNutrientBar(365, start_y + 56, "Carbs", f->carbs, t, RED);
                snprintf(t, sizeof(t), "%.1f g", f->fats * FATS_SCALE);
                DrawNutrientBar(365, start_y + 76, "Fats", f->fats, t, GOLD);
                snprintf(t, sizeof(t), "%.0f kcal", f->calories * CALORIES_SCALE);
                DrawNutrientBar(365, start_y + 96, "Calories", f->calories, t, GREEN);

                start_y += 140;
            }
        } else {
            DrawText("Search for a food, select it, then click\nFIND SUBSTITUTES (or press Enter).", 400, 220, 16, GRAY);
        }

        EndDrawing();
    }

    // 3. De-allocate system allocations and close down process loops safely
    CloseWindow();
    return 0;
}







