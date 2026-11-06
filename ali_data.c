#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "food_structures.h"

// Function to load and parse the comprehensive USDA CSV dataset
int load_nutritional_data(const char *filename, FoodItem dataset[], int max_size) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open dataset file %s\n", filename);
        return -1;
    }

    char line[1024];
    int count = 0;

    // Skip the header row
    if (!fgets(line, sizeof(line), file)) {
        fclose(file);
        return 0;
    }

    // Read file line by line
    while (fgets(line, sizeof(line), file) && count < max_size) {
        line[strcspn(line, "\r\n")] = 0; // Remove newline
        if (strlen(line) == 0) continue;

        // Use strtok to loop through CSV columns matching comprehensive_foods_usda.csv
        char *token = strtok(line, ","); // Column 1: fdc_id (skip)
        
        token = strtok(NULL, ",");      // Column 2: food_name
        if (!token) continue;
        strncpy(dataset[count].name, token, sizeof(dataset[count].name) - 1);
        dataset[count].name[sizeof(dataset[count].name) - 1] = '\0';

        // Skip intermediate columns (data_type, category, brand, etc.) until we reach nutrients
        // Depending on exact USDA layout, we parse numeric values:
        // Let's grab the next tokens safely
        for (int i = 0; i < 15; i++) { // adjust loop or direct tokens based on column position
            token = strtok(NULL, ",");
            if (!token) break;
            
            // Example position mapping (we will verify with your test run):
            if (i == 5) dataset[count].protein = atof(token);
            if (i == 7) dataset[count].carbs = atof(token);
            if (i == 9) dataset[count].fats = atof(token);
            if (i == 11) dataset[count].calories = atof(token);
        }

        count++;
    }

    fclose(file);
    return count;
}

int main() {
    FoodItem dataset[10];
    int total_loaded = load_nutritional_data("nutrition_dataset.csv", dataset, 10);

    printf("\n=== ALI'S DATA PIPELINE TEST ===\n");
    if (total_loaded > 0) {
        printf("Successfully parsed %d items from dataset:\n\n", total_loaded);
        for (int i = 0; i < total_loaded; i++) {
            printf("%d. %s\n", i + 1, dataset[i].name);
            printf("   [Protein: %.2f | Carbs: %.2f | Fats: %.2f | Calories: %.2f]\n",
                   dataset[i].protein, dataset[i].carbs, dataset[i].fats, dataset[i].calories);
        }
    } else {
        printf("No data loaded.\n");
    }

    return 0;
}