#include <stdio.h>
#include <math.h>
#include <string.h>
#include "food_structures.h"
//Name:Muhammad Qasim Khan
//Class:BDS-1A
//roll no:26k-2529

// 1. Calculates the Euclidean Distance between two vectors in 4D space
double calculate_vector_distance(FoodItem item1, FoodItem item2) {
    double p_diff   = item1.protein - item2.protein;
    double c_diff   = item1.carbs - item2.carbs;
    double f_diff   = item1.fats - item2.fats;
    double cal_diff = item1.calories - item2.calories;

    return sqrt(pow(p_diff, 2) + pow(c_diff, 2) + pow(f_diff, 2) + pow(cal_diff, 2));
}

// 2. Sorts calculated distances in ascending order (closest matches first)
void sort_substitutes(FoodItem dataset[], double distances[], int size) {
    int i, j, min_idx;
    for (i = 0; i < size - 1; i++) {
        min_idx = i;
        for (j = i + 1; j < size; j++) {
            if (distances[j] < distances[min_idx]) {
                min_idx = j;
            }
        }
        double temp_dist = distances[min_idx];
        distances[min_idx] = distances[i];
        distances[i] = temp_dist;

        FoodItem temp_food = dataset[min_idx];
        dataset[min_idx] = dataset[i];
        dataset[i] = temp_food;
    }
}

// 3. Master function: scans the WHOLE dataset once and keeps the 3 closest matches.
//    (The old version copied everything into fixed 1000-item arrays, which would
//    crash with the real 39,000-food dataset. This version needs no big arrays.)
void calculate_substitutes(FoodItem target, FoodItem dataset[], int size, FoodItem results[3]) {
    double best_dist[3] = { 1e30, 1e30, 1e30 };
    int found = 0;

    // Clear the result slots so unused ones never show garbage
    for (int k = 0; k < 3; k++) {
        results[k].name[0] = '\0';
        results[k].protein = results[k].carbs = results[k].fats = results[k].calories = 0.0;
    }

    for (int i = 0; i < size; i++) {
        // Skip comparing the exact same ingredient to itself
        if (strcmp(target.name, dataset[i].name) == 0) {
            continue;
        }

        double d = calculate_vector_distance(target, dataset[i]);
        if (d >= best_dist[2]) continue;           // not in the top 3

        // Insert into the sorted top-3 list
        int pos = 2;
        while (pos > 0 && d < best_dist[pos - 1]) {
            best_dist[pos] = best_dist[pos - 1];
            results[pos]   = results[pos - 1];
            pos--;
        }
        best_dist[pos] = d;
        results[pos]   = dataset[i];
        if (found < 3) found++;
    }
}