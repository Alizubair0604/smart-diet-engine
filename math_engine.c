#include <stdio.h>
#include <math.h>
#include <string.h>
#include "food_structures.h"
//Name:Muhammad Qasim Khan
//Class:BDS-1A
//roll no:26k-2529

// 1. Calculates the Euclidean Distance between two vectors(in this case two food items) in 4D space
//---The smaller the distance between the nutritional values of food means the more those two foods are simmilar
//---The structure separates each nutritional value of the food and treats it as coordinates in 4 dimentional space
//---Then it checks the distance between the coordinates of two food items
double calculate_vector_distance(FoodItem item1, FoodItem item2) {
    double p_diff   = item1.protein - item2.protein;
    double c_diff   = item1.carbs - item2.carbs;
    double f_diff   = item1.fats - item2.fats;
    double cal_diff = item1.calories - item2.calories;

    return sqrt(pow(p_diff, 2) + pow(c_diff, 2) + pow(f_diff, 2) + pow(cal_diff, 2));
}

// 2. Sorts calculated distances in ascending order (closest matches first)
//---It is a sorting algorithm where it sorts the value in ascending order along with their respective distances so each dataitem is sorted with its distance.
void sort_substitutes(FoodItem dataset[], double distances[], int size) {
    int i, j, min_idx;
    //The outer loop here calls out each index value.At the end of each pass i has the minimum value.
    for (i = 0; i < size - 1; i++) { 
        min_idx = i;
    //Inner loop compares and updates
        for (j = i + 1; j < size; j++) {
            if (distances[j] < distances[min_idx]) {
                min_idx = j;
            }
        }
        //---swaping algorithm after every outer loop
        double temp_dist = distances[min_idx];
        distances[min_idx] = distances[i];
        distances[i] = temp_dist;
        //---swapping of the dataset of the respective distances so each pair is sorted
        FoodItem temp_food = dataset[min_idx];
        dataset[min_idx] = dataset[i];
        dataset[i] = temp_food;
    }
}

// 3. Master function: scans the WHOLE dataset once and keeps the 3 closest matches.
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
        //majority of the list is rejected here
        if (d >= best_dist[2]) continue;           // not in the top 3

        // Insertion into the sorted top-3 list
        int pos = 2;
        while (pos > 0 && d < best_dist[pos - 1]) {
            best_dist[pos] = best_dist[pos - 1];
            results[pos]   = results[pos - 1];
            pos--;
        }
        //final sort
        best_dist[pos] = d;
        results[pos]   = dataset[i];
        if (found < 3) found++;
    }
}
