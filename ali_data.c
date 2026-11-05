#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "food_structures.h"

// Roll No: 26k-2521 | Name: Ali Zubair
// Section: BDS-1A | Data Pipeline Layer

int load_nutritional_data(const char* filename, FoodItem dataset[], int max_size) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not open data file %s\n", filename);
        return 0;
    }

    char line[256];
    int count = 0;

    // Skip CSV header line
    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        return 0;
    }

    // Read lines
    while (fgets(line, sizeof(line), file) != NULL && count < max_size) {
        line[strcspn(line, "\r\n")] = 0;

        char* token = strtok(line, ",");
        if (token == NULL) continue;
        
        // Safely copy string into name array
        strncpy(dataset[count].name, token, sizeof(dataset[count].name) - 1);
        dataset[count].name[sizeof(dataset[count].name) - 1] = '\0';

        token = strtok(NULL, ",");
        dataset[count].protein = token ? atof(token) : 0.0;

        token = strtok(NULL, ",");
        dataset[count].carbs = token ? atof(token) : 0.0;

        token = strtok(NULL, ",");
        dataset[count].fats = token ? atof(token) : 0.0;

        token = strtok(NULL, ",");
        dataset[count].calories = token ? atof(token) : 0.0;

        // Data Normalization
        if (dataset[count].protein > 1.0) dataset[count].protein /= 100.0;
        if (dataset[count].carbs > 1.0) dataset[count].carbs /= 100.0;
        if (dataset[count].fats > 1.0) dataset[count].fats /= 100.0;
        if (dataset[count].calories > 1.0) dataset[count].calories /= 100.0;

        count++;
    }

    fclose(file);
    return count;
}

int main() {
    FoodItem dataset[100];
    int total_loaded = load_nutritional_data("nutrition_dataset.csv", dataset, 100);

    printf("\n=== ALI'S DATA PIPELINE LAYER TEST ===\n");
    printf("Successfully parsed %d items from CSV:\n\n", total_loaded);

    for (int i = 0; i < total_loaded; i++) {
        printf("%d. %s\n", i + 1, dataset[i].name);
        printf("   [Protein: %.2f | Carbs: %.2f | Fats: %.2f | Calories: %.2f]\n\n",
               dataset[i].protein, dataset[i].carbs, dataset[i].fats, dataset[i].calories);
    }

    return 0;
}