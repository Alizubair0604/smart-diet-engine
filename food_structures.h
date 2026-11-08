#ifndef FOOD_STRUCTURES_H
#define FOOD_STRUCTURES_H

// Nutrient values stored in FoodItem are NORMALIZED to the range 0.0 - 1.0:
//   protein = grams / 100,  carbs = grams / 100,  fats = grams / 100,
//   calories = kcal / 900   (all per 100 g serving).
// Multiply by these scales to get real units back for display.
#define PROTEIN_SCALE  100.0
#define CARBS_SCALE    100.0
#define FATS_SCALE     100.0
#define CALORIES_SCALE 900.0
typedef struct
{
    char name[100];
    double protein;
    double carbs;
    double fats;
    double calories;
} FoodItem;

int load_nutritional_data(const char *filename, FoodItem dataset[], int max_size);
void calculate_substitutes(FoodItem target, FoodItem dataset[], int size, FoodItem results[3]);

#endif