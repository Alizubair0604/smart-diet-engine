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
    // Allocation for up to 1000 items from Ali's CSV file
    FoodItem dataset[1000]; 
    int total_items = 0;

    // Call Ali's parsing function to read the dataset file
    // Note: Ali must implement this function inside food_structures.h or his C file
    total_items = load_data_from_csv("nutrition.csv", dataset);

    if (total_items == 0) {
        printf("Error: Could not load data from nutrition.csv or file is empty.\n");
        return 1;
    }

    // Get target input dynamically from the user
    FoodItem target;
    printf("Enter the food item you want to find a substitute for: ");
    fgets(target.name, sizeof(target.name), stdin);
    target.name[strcspn(target.name, "\n")] = '\0'; 

 // Cleans the hidden newline character from pressing Enter

    // Find the original target's macros from the database to populate your search vector
    int found_target = 0;
    for (int i = 0; i < total_items; i++) {
        if (strcmp(dataset[i].name, target.name) == 0) {
            target = dataset[i];
            found_target = 1;
            break;
        }
    }

    if (!found_target) {
        printf("Food item '%s' not found in the database.\n", target.name);
        return 1;
    }

    // Run your mathematical substitution calculation matrix
    FoodItem results[3];
    calculate_substitutes(target, dataset, total_items, results);

    // Display the results dynamically
    printf("\n--- Top Healthy Substitutes for %s ---\n", target.name);
    for(int i = 0; i < 3 && i < total_items - 1; i++) {
        printf("%d. %s (Protein: %.2f, Carbs: %.2f, Fats: %.2f, Calories: %.2f)\n", 
               i + 1, results[i].name, results[i].protein, results[i].carbs, results[i].fats, results[i].calories);
    }

    return 0;
}
