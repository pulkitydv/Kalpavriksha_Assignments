#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_INITIAL_PRODUCTS 100
#define MAX_NAME_LENGTH 50
#define MAX_PRODUCT_ID 10000
#define MAX_PRODUCT_PRICE 100000.0
#define MAX_PRODUCT_QUANTITY 1000000

typedef struct{
    int productID;
    char* productName;
    float productPrice;
    int productQuantity;
} Product;

typedef struct{
    Product* products;
    int currentNumberOfProducts;
} Inventory;

int getValidInteger(){
    int integer;

    while(1){
        if(scanf("%d", &integer) != 1){
            printf("Please Enter the valid integer only: ");
            while(getchar() != '\n');
        } else {
            if(getchar() != '\n'){
                printf("Please Enter the valid integer only: ");
                while(getchar() != '\n');
            } else {
                return integer;
            }
        }
    }
}

float getValidFloat(){
    float number;

    while(1){
        if(scanf("%f", &number) != 1){
            printf("Please Enter a valid number: ");
            while(getchar() != '\n');
        } else {
            if(getchar() != '\n'){
                printf("Please Enter a valid number: ");
                while(getchar() != '\n');
            } else {
                return number;
            }
        }
    }
}

int getInitialNumberOfProducts(){
    int numberOfProducts;

    while(1){
        numberOfProducts = getValidInteger();
        if (numberOfProducts < 1 || numberOfProducts > MAX_INITIAL_PRODUCTS)
        {
            printf("Please enter a number between (1 - 100): ");
        }
        else
        {
            return numberOfProducts;
        }
    }
}

int idExists(Inventory *inventory, int id){
    for(int product = 0; product < inventory->currentNumberOfProducts; product++){
        if(id == (inventory->products + product)->productID){
            return 1;
        }
    }
    return 0;
}

int getProductID(Inventory *inventory){
    int productID;

    while(1){
        productID = getValidInteger();
        if(productID < 1 || productID > MAX_PRODUCT_ID){
            printf("Invalid ID. Enter ID between 1 - 10000: ");
        } else if(idExists(inventory, productID)){
            printf("ID Already Exists! Try again: ");
        } else {
            return productID;
        }
    }
}

char* getProductName(){
    char* productName = calloc(MAX_NAME_LENGTH, sizeof(char));
    if(productName == NULL){
        printf("Memory allocation failed!\n");
        return NULL;
    }
    fgets(productName, MAX_NAME_LENGTH, stdin);
    productName[strcspn(productName, "\n")] = '\0';
    return productName;
}

float getProductPrice(){
    float productPrice;

    while(1){
        productPrice = getValidFloat();
        if(productPrice < 0.0 || productPrice > MAX_PRODUCT_PRICE){
            printf("Invalid Price! Please enter price between 0-100000: ");
        } else {
            return productPrice;
        }
    }
}

int getProductQuantity(){
    int productQuantity;

    while(1){
        productQuantity = getValidInteger();
        if(productQuantity < 0 || productQuantity > MAX_PRODUCT_QUANTITY){
            printf("Invalid Quantity! Please enter quantity between 0-1000000: ");
        } else {
            return productQuantity;
        }
    }
}

void getProductDetails(Inventory *inventory, int productNumber){
    printf("Product ID: ");
    (inventory->products + productNumber)->productID = getProductID(inventory);
    printf("Product Name: ");
    (inventory->products + productNumber)->productName = getProductName();
    if((inventory->products + productNumber)->productName == NULL){
        return;
    }
    printf("Product Price: ");
    (inventory->products + productNumber)->productPrice = getProductPrice();
    printf("Product Quantity: ");
    (inventory->products + productNumber)->productQuantity = getProductQuantity();
}

void showMenu()
{
    printf("\n========= INVENTORY MENU =========\n");
    printf("1. Add New Product\n");
    printf("2. View All Products\n");
    printf("3. Update Quantity\n");
    printf("4. Search Product by ID\n");
    printf("5. Search Product by Name\n");
    printf("6. Search Product by Price Range\n");
    printf("7. Delete Product\n");
    printf("8. Exit\n");
}

Product *addNewProduct(Inventory *inventory)
{
    Product *tempPtr = realloc(inventory->products, (inventory->currentNumberOfProducts + 1) * sizeof(Product));
    if (tempPtr != NULL)
    {
        inventory->products = tempPtr;
        inventory->currentNumberOfProducts++;
    }
    else
    {
        printf("\nFailed to reallocate memory!\n");
        return NULL;
    }
    printf("\nEnter new product details: \n");
    getProductDetails(inventory, inventory->currentNumberOfProducts - 1);
    printf("Product added successfully!\n");
    return inventory->products;
}

void productDetails(Product* product){
    printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n", 
    product->productID, product->productName, product->productPrice, product->productQuantity);
}

void viewAllProducts(Inventory *inventory){
    printf("\n========= PRODUCT LIST =========\n");
    for(int product = 0; product < inventory->currentNumberOfProducts; product++){
        productDetails(inventory->products + product);
    }
}

void updateQuantity(Inventory *inventory){
    int newQuantity;
    int id;
    printf("\nEnter Product ID to update quantity: ");
    id = getValidInteger();
    if(!idExists(inventory, id)){
        printf("ID does not exist!\n");
        return; 
    }
    printf("Enter new Quantity: ");
    newQuantity = getProductQuantity();

    for(int product = 0; product < inventory->currentNumberOfProducts; product++){
        if(id == (inventory->products + product)->productID){
            (inventory->products + product)->productQuantity = newQuantity;
        }
    }
    printf("Quantity updated successfully!\n");
}

void searchProductByID(Inventory *inventory){
    int id;
    printf("\nEnter Product ID to search: ");
    id = getValidInteger();
    if (!idExists(inventory, id))
    {
        printf("ID does not exist!\n");
        return;
    }

    printf("Product Found: ");
    for (int product = 0; product < inventory->currentNumberOfProducts; product++)
    {
        if (id == (inventory->products + product)->productID)
        {
            productDetails(inventory->products + product);
        }
    }
}

int isSubstring(char* substring, char* string){
    while(*string != '\0'){
        char* stringPtr = string;
        char* substringPtr = substring;

        while(*stringPtr != '\0' && *substringPtr != '\0' && tolower(*stringPtr) == tolower(*substringPtr)){
            stringPtr++;
            substringPtr++;
        }
        if(*substringPtr == '\0'){
            return 1;
        }
        string++;
    }
    return 0;
}

void searchProductByName(Inventory *inventory){
    printf("\nEnter name to search (partial allowed): ");
    char* nameToFind = getProductName();
    int found = 0;

    for(int product = 0; product < inventory->currentNumberOfProducts; product++){
        if(isSubstring(nameToFind, (inventory->products+product)->productName)){
            if(!found){
                printf("Products found: \n");
                found = 1;
            }
            productDetails(inventory->products + product);
        }
    }
    if(!found){
        printf("No Products found!\n");
    }
    free(nameToFind);
}

void searchProductByPriceRange(Inventory *inventory)
{
    float minPrice, maxPrice;
    printf("\nEnter the minimum price: ");
    minPrice = getValidFloat();
    printf("Enter the maximum price: ");
    maxPrice = getValidFloat(); 
    if(minPrice > maxPrice){
        printf("Invalid range! Minimum price cannot be greater than maximum price.\n");
        return;
    }

    int isThereAnyProduct = 0;

    for (int product = 0; product < inventory->currentNumberOfProducts; product++)
    {
        if ((inventory->products + product)->productPrice >= minPrice && (inventory->products + product)->productPrice <= maxPrice)
        {
            if(!isThereAnyProduct){
                printf("\nProducts in price range: \n");
                isThereAnyProduct = 1;
            }
            productDetails(inventory->products + product);
        }
    }
    if (!isThereAnyProduct)
    {
        printf("No Products found in this price range.\n");
    }
}

void shiftProducts(Inventory *inventory, int index){
    while(index + 1 < inventory->currentNumberOfProducts){
        *(inventory->products+index) = *(inventory->products+index + 1);
        index++;
    }
}

Product* deleteProduct(Inventory *inventory){
    printf("\nEnter Product ID to delete: ");
    int idToDelete = getValidInteger();
    if (!idExists(inventory, idToDelete))
    {
        printf("ID does not exist!\n");
        return inventory->products;
    }

    for(int product = 0; product < inventory->currentNumberOfProducts; product++){
        if( (inventory->products+product)->productID == idToDelete){
            free((inventory->products + product)->productName);
            shiftProducts(inventory, product);
            inventory->currentNumberOfProducts--;
            Product *tempPtr = realloc(inventory->products, inventory->currentNumberOfProducts * sizeof(Product));
            if (tempPtr != NULL)
            {
                inventory->products = tempPtr;
            }
            else
            {
                printf("Item deleted. Failed to reallocate memory.\n");
                return inventory->products;
            }
            printf("Product deleted successfully!\n");
        }
    }
    return inventory->products;
}

void freeMemory(Inventory *inventory){
    for(int product = 0; product < inventory->currentNumberOfProducts; product++){
        free((inventory->products+product)->productName);
    }
    free(inventory->products);
    inventory->products = NULL;
}

void menuOperations(Inventory *inventory){
    int choice = 0;

    while(choice != 8){
        showMenu();
        printf("Enter your choice: ");
        choice = getValidInteger();
        switch (choice)
        {
        case 1:
            inventory->products = addNewProduct(inventory);
            break;
        case 2:
            viewAllProducts(inventory);
            break;
        case 3:
            updateQuantity(inventory);
            break;
        case 4:
            searchProductByID(inventory);
            break;
        case 5:
            searchProductByName(inventory);
            break;
        case 6:
            searchProductByPriceRange(inventory);
            break;
        case 7:
            inventory->products = deleteProduct(inventory);
            break;
        case 8:
            freeMemory(inventory);
            printf("Memory released successfully. Exiting program...");
            return;
        default:
            break;
        }
    }
}

int main(){

    Inventory inventory;  
    inventory.currentNumberOfProducts = 0;

    printf("Enter initial number of products: ");
    int initialProducts = getInitialNumberOfProducts();

    inventory.products = calloc(initialProducts, sizeof(Product));
    if(inventory.products == NULL){
        printf("Memory allocation failed!");
        return 1;
    }
    for(int productNumber = 0; productNumber < initialProducts; productNumber++){
        printf("\nEnter details for product %d: \n", productNumber + 1);
        getProductDetails(&inventory, productNumber);
        inventory.currentNumberOfProducts++;
    }

    menuOperations(&inventory);
    return 0;
}
