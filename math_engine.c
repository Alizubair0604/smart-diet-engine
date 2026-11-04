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
        // Swap distance values
        double temp_dist = distances[min_idx];
        distances[min_idx] = distances[i];
        distances[i] = temp_dist;
        
        // Parallel swap matching FoodItem structures
        FoodItem temp_food = dataset[min_idx];
        dataset[min_idx] = dataset[i];
        dataset[i] = temp_food;
    }
}

// 3. Master function to search the dataset and isolate the top 3 best fits
void calculate_substitutes(FoodItem target, FoodItem dataset[], int size, FoodItem results[3]) {
    double distances[1000]; 
    FoodItem temp_dataset[1000]; 
    int valid_count = 0;

    for (int i = 0; i < size; i++) {
        // Skip comparing the exact same ingredient to itself
        if (strcmp(target.name, dataset[i].name) == 0) {
            continue;
        }
        
        temp_dataset[valid_count] = dataset[i];
        distances[valid_count] = calculate_vector_distance(target, dataset[i]);
        valid_count++;
    }

    sort_substitutes(temp_dataset, distances, valid_count);

    // Extract the top 3 closest structural vector matches
    for (int i = 0; i < 3 && i < valid_count; i++) {
        results[i] = temp_dataset[i];
    }
}

// 4. Standalone Test Loop (To run and confirm your math on your machine)
int main() {
    FoodItem target;
    strcpy(target.name, "White Rice");
    target.protein = 0.1; target.carbs = 0.9; target.fats = 0.0; target.calories = 0.6;

    FoodItem mock_dataset[3];
    
    strcpy(mock_dataset[0].name, "Cauliflower Rice"); // Closest vector match
    mock_dataset[0].protein = 0.1; mock_dataset[0].carbs = 0.2; mock_dataset[0].fats = 0.0; mock_dataset[0].calories = 0.1;

    strcpy(mock_dataset[1].name, "Avocado");
    mock_dataset[1].protein = 0.1; mock_dataset[1].carbs = 0.3; mock_dataset[1].fats = 0.7; mock_dataset[1].calories = 0.5;

    strcpy(mock_dataset[2].name, "Potato Fries");
    mock_dataset[2].protein = 0.1; mock_dataset[2].carbs = 0.95; mock_dataset[2].fats = 0.4; mock_dataset[2].calories = 0.8;

    FoodItem results[3];
    calculate_substitutes(target, mock_dataset, 3, results);

    printf("--- Top Healthy Substitutes for %s ---\n", target.name);
    for(int i = 0; i < 3; i++) {
        printf("%d. %s (Protein: %.2f, Carbs: %.2f, Fats: %.2f)\n", 
               i + 1, results[i].name, results[i].protein, results[i].carbs, results[i].fats);
    }
    return 0;
}
