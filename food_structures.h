#ifndef FOOD_STRUCTURES_H
#define FOOD_STRUCTURES_H

typedef struct {
    char name[100]; // Fixed: string array to hold names like "Cauliflower Rice"
    double protein;
    double carbs;
    double fats;
    double calories;
} FoodItem;

#endif