#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_INITIAL_PRODUCTS 100
#define MAX_NAME_LENGTH 50
#define MAX_PRODUCT_ID 10000
#define MAX_PRODUCT_PRICE 100000.0
#define MAX_PRODUCT_QUANTITY 1000000

int currentNumberOfProducts = 0;

typedef struct{
    int productID;
    char* productName;
    float productPrice;
    int productQuantity;
} Product;

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

int idExists(Product *products, int id){
    for(int product = 0; product < currentNumberOfProducts; product++){
        if(id == (products+product)->productID){
            return 1;
        }
    }
    return 0;
}

int getProductID(Product *products){
    int productID;

    while(1){
        productID = getValidInteger();
        if(productID < 1 || productID > MAX_PRODUCT_ID){
            printf("Invalid ID. Enter ID between 1 - 10000: ");
        } else if(idExists(products, productID)){
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

void getProductDetails(Product *products, int productNumber){
    printf("Product ID: ");
    (products+productNumber)->productID = getProductID(products);
    printf("Product Name: ");
    (products+productNumber)->productName = getProductName();
    if((products+productNumber)->productName == NULL){
        return;
    }
    printf("Product Price: ");
    (products+productNumber)->productPrice = getProductPrice();
    printf("Product Quantity: ");
    (products+productNumber)->productQuantity = getProductQuantity();
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

Product *addNewProduct(Product *products)
{
    Product *tempPtr = realloc(products, (currentNumberOfProducts + 1) * sizeof(Product));
    if (tempPtr != NULL)
    {
        products = tempPtr;
        currentNumberOfProducts++;
    }
    else
    {
        printf("\nFailed to reallocate memory!\n");
        return products;
    }
    printf("\nEnter new product details: \n");
    getProductDetails(products, currentNumberOfProducts - 1);
    printf("Product added successfully!\n");
    return products;
}

void productDetails(Product* product){
    printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n", 
    product->productID, product->productName, product->productPrice, product->productQuantity);
}

void viewAllProducts(Product* products){
    printf("\n========= PRODUCT LIST =========\n");
    for(int product = 0; product < currentNumberOfProducts; product++){
        productDetails(products + product);
    }
}

void updateQuantity(Product* products){
    int newQuantity;
    int id;
    printf("\nEnter Product ID to update quantity: ");
    id = getValidInteger();
    if(!idExists(products, id)){
        printf("ID does not exist!\n");
        return; 
    }
    printf("Enter new Quantity: ");
    newQuantity = getProductQuantity();

    for(int product = 0; product < currentNumberOfProducts; product++){
        if(id == (products+product)->productID){
            (products + product)->productQuantity = newQuantity;
        }
    }
    printf("Quantity updated successfully!\n");
}

void searchProductByID(Product* products){
    int id;
    printf("\nEnter Product ID to search: ");
    id = getValidInteger();
    if (!idExists(products, id))
    {
        printf("ID does not exist!\n");
        return;
    }

    printf("Product Found: ");
    for (int product = 0; product < currentNumberOfProducts; product++)
    {
        if (id == (products + product)->productID)
        {
            productDetails(products + product);
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

void searchProductByName(Product* products){
    printf("\nEnter name to search (partial allowed): ");
    char* nameToFind = getProductName();
    int found = 0;

    for(int product = 0; product < currentNumberOfProducts; product++){
        if(isSubstring(nameToFind, (products+product)->productName)){
            if(!found){
                printf("Products found: \n");
                found = 1;
            }
            productDetails(products + product);
        }
    }
    if(!found){
        printf("No Products found!\n");
    }
    free(nameToFind);
}

void searchProductByPriceRange(Product *products)
{
    float minPrice, maxPrice;
    printf("\nEnter the minimum price: ");
    minPrice = getValidFloat();
    printf("Enter the maximum price: ");
    maxPrice = getValidFloat();
    int isThereAnyProduct = 0;

    for (int product = 0; product < currentNumberOfProducts; product++)
    {
        if ((products + product)->productPrice >= minPrice && (products + product)->productPrice <= maxPrice)
        {
            if(!isThereAnyProduct){
                printf("\nProducts in price range: \n");
                isThereAnyProduct = 1;
            }   
            productDetails(products + product);
        }
    }
    if (!isThereAnyProduct)
    {
        printf("No Products found in this price range.\n");
    }
}

void shiftProducts(Product* product, int index){
    while(index + 1 < currentNumberOfProducts){
        product->productID = (product+1)->productID;
        product->productName = (product+1)->productName;
        product->productPrice = (product+1)->productPrice;
        product->productQuantity = (product+1)->productQuantity;
        index++;
        product++;
    }
}

Product* deleteProduct(Product* products){
    printf("\nEnter Product ID to delete: ");
    int idToDelete = getValidInteger();
    if (!idExists(products, idToDelete))
    {
        printf("ID does not exist!\n");
        return products;
    }

    for(int product = 0; product < currentNumberOfProducts; product++){
        if( (products+product)->productID == idToDelete){
            free((products + product)->productName);
            shiftProducts((products+product), product);
            currentNumberOfProducts--;
            Product *tempPtr = realloc(products, currentNumberOfProducts * sizeof(Product));
            if (tempPtr != NULL)
            {
                products = tempPtr;
            }
            else
            {
                printf("Item deleted. Failed to reallocate memory.\n");
                return products;
            }
            printf("Product deleted successfully!\n");
        }
    }
    return products;
}

void freeMemory(Product* products){
    for(int product = 0; product < currentNumberOfProducts; product++){
        free((products+product)->productName);
    }
    free(products);
    products = NULL;
}

void menuOperations(Product* products){
    int choice = 0;

    while(choice != 8){
        showMenu();
        printf("Enter your choice: ");
        choice = getValidInteger();
        switch (choice)
        {
        case 1:
            products = addNewProduct(products);
            break;
        case 2:
            viewAllProducts(products);
            break;
        case 3:
            updateQuantity(products);
            break;
        case 4:
            searchProductByID(products);
            break;
        case 5:
            searchProductByName(products);
            break;
        case 6:
            searchProductByPriceRange(products);
            break;
        case 7:
            products = deleteProduct(products);
            break;
        case 8:
            freeMemory(products);
            printf("Memory released successfully. Exiting program...");
            return;
        default:
            break;
        }
    }
}

int main(){

    int initialProducts;
    printf("Enter initial number of products: ");
    initialProducts = getInitialNumberOfProducts();

    Product *products = calloc(initialProducts, sizeof(Product));
    if(products == NULL){
        printf("Memory allocation failed!");
        return 1;
    }
    for(int productNumber = 0; productNumber < initialProducts; productNumber++){
        printf("\nEnter details for product %d: \n", productNumber + 1);
        getProductDetails(products, productNumber);
        currentNumberOfProducts++;
    }

    menuOperations(products);
    return 0;
}
