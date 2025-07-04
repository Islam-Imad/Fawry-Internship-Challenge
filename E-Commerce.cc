#include <bits/stdc++.h>
using namespace std;

class Product;
class WeightedProduct;
class ExpirableProduct;
class WeightedExpirableProduct;

class ProductVisitor
{
public:
    virtual void visit(Product *product) = 0;
    virtual void visit(WeightedProduct *product) = 0;
    virtual void visit(ExpirableProduct *product) = 0;
    virtual void visit(WeightedExpirableProduct *product) = 0;
    virtual ~ProductVisitor() = default;
};

class Product
{
private:
    string name;
    double price;
    double fees;
    int quantity;

public:
    Product(string name, double price, double fees, int quantity) : name(name), price(price), fees(fees), quantity(quantity) {}
    string getName() const { return name; }
    double getPrice() const { return price; }
    double getFees() const { return fees; }
    int getQuantity() const { return quantity; }
    void setQuantity(int quantity) { this->quantity = quantity; }

    void consume_quantity(int requestedQuantity)
    {
        if (requestedQuantity <= quantity)
        {
            quantity -= requestedQuantity;
        }
        else
        {
            cout << "Requested quantity exceeds available quantity for product: " << name << endl;
        }
    }

    bool checkAvailableQuantity(int requestedQuantity) const
    {
        return requestedQuantity <= quantity;
    }

    double getSubTotal(int requestedQuantity) const
    {
        return price * requestedQuantity;
    }

    double getTotalFees(int requestedQuantity) const
    {
        return fees * getSubTotal(requestedQuantity);
    }

    double getTotal(int requestedQuantity) const
    {
        return getSubTotal(requestedQuantity) + getTotalFees(requestedQuantity);
    }

    virtual void accept(ProductVisitor *visitor)
    {
        visitor->visit(this);
    }
};

class WeightedProduct : public Product
{
private:
    double weight;

public:
    WeightedProduct(string name, double price, double fees, int quantity, double weight)
        : Product(name, price, fees, quantity), weight(weight) {}
    double getWeight() const { return weight; }
    virtual void accept(ProductVisitor *visitor)
    {
        visitor->visit(this);
    }
};

class ExpirableProduct : public Product
{
private:
    string expiryDate;

public:
    ExpirableProduct(string name, double price, double fees, int quantity, string expiryDate)
        : Product(name, price, fees, quantity), expiryDate(expiryDate) {}
    string getExpiryDate() const { return expiryDate; }
    virtual void accept(ProductVisitor *visitor)
    {
        visitor->visit(this);
    }
};

class WeightedExpirableProduct : public Product
{
private:
    double weight;
    string expiryDate;

public:
    WeightedExpirableProduct(string name, double price, double fees, int quantity, double weight, string expiryDate)
        : Product(name, price, fees, quantity), weight(weight), expiryDate(expiryDate) {}
    double getWeight() const { return weight; }
    string getExpiryDate() const { return expiryDate; }
    virtual void accept(ProductVisitor *visitor)
    {
        visitor->visit(this);
    }
};

class WeightVisitor : public ProductVisitor
{
private:
    double weight;

public:
    WeightVisitor() : weight(0.0) {}
    void visit(Product *product) override
    {
        weight = 0;
    }
    void visit(WeightedProduct *product) override
    {
        weight = product->getWeight();
    }
    void visit(ExpirableProduct *product) override
    {
        weight = 0;
    }
    void visit(WeightedExpirableProduct *product) override
    {
        weight = product->getWeight();
    }

    double getWeight() const
    {
        return weight;
    }
};

class ExpiryVisitor : public ProductVisitor
{
private:
    int expired;
    string currentDate;

public:
    ExpiryVisitor(string currentDate) : expired(0), currentDate(currentDate) {}

    void visit(Product *product) override
    {
        expired = 0;
    }

    void visit(ExpirableProduct *product) override
    {
        if (product->getExpiryDate() < currentDate)
        {
            expired = 1;
        }
        else
        {
            expired = 0;
        }
    }

    void visit(WeightedExpirableProduct *product) override
    {
        if (product->getExpiryDate() < currentDate)
        {
            expired = 1;
        }
        else
        {
            expired = 0;
        }
    }

    void visit(WeightedProduct *product) override
    {
        expired = 0;
    }

    bool isExpired() const
    {
        return expired == 1;
    }
};

class Cart
{
private:
    vector<pair<Product *, int>> products;
    string currentDate;

public:
    Cart(string currentDate = "2023-10-01") : currentDate(currentDate) {}
    bool addProduct(Product *product, int requestedQuantity)
    {
        if (product->checkAvailableQuantity(requestedQuantity))
        {
            products.push_back(make_pair(product, requestedQuantity));
            ExpiryVisitor expiryVisitor(currentDate);
            product->accept(&expiryVisitor);
            if (expiryVisitor.isExpired())
            {
                cout << "Product is expired: " << product->getName() << endl;
                return false;
            }
            return true;
        }
        else
        {
            cout << "Requested quantity exceeds available quantity for product: " << product->getName() << endl;
            return false;
        }
    }

    double getTotal() const
    {
        double total = 0.0;
        for (const auto &[product, quantity] : products)
        {
            total += product->getTotal(quantity);
        }
        return total;
    }

    double getTotalFees() const
    {
        double totalFees = 0.0;
        for (const auto &[product, quantity] : products)
        {
            totalFees += product->getTotalFees(quantity);
        }
        return totalFees;
    }

    double getSubTotal() const
    {
        double subTotal = 0.0;
        for (const auto &[product, quantity] : products)
        {
            subTotal += product->getSubTotal(quantity);
        }
        return subTotal;
    }

    void printCart() const
    {
        double totalWeight = 0.0;
        WeightVisitor weightVisitor;
        cout << "quantity   product   unit_weight   total_weight\n";
        for (const auto &[product, quantity] : products)
        {
            product->accept(&weightVisitor);
            double weight = weightVisitor.getWeight();
            totalWeight += weight * quantity;
            cout << quantity << "x " << product->getName() << "   " << weight << "   " << (weight * quantity) << "\n";
        }
        cout << "Total Weight: " << totalWeight << "\n";
        cout << "****************************\n";
        cout << "quantity   product   unit_price   sub_total\n";
        for (const auto &[product, quantity] : products)
        {
            cout << quantity << "x " << product->getName() << "   " << product->getPrice() << "   " << product->getSubTotal(quantity) << "\n";
        }
        cout << "****************************\n";
        cout << "Sub Total: " << getSubTotal() << "\n";
        cout << "Total Fees: " << getTotalFees() << "\n";
        cout << "Total: " << getTotal() << "\n";
    }

    void purchase()
    {
        for (auto &[product, quantity] : products)
        {
            product->consume_quantity(quantity);
        }
        products.clear();
    }

    int countItems() const
    {
        return products.size();
    }
};

class Customer
{
private:
    string name;
    double balance;

public:
    Customer(string name, double balance) : name(name), balance(balance) {}
    string getName() const { return name; }
    double getBalance() const { return balance; }
    void setBalance(double balance) { this->balance = balance; }

    bool canAfford(double amount) const
    {
        return balance >= amount;
    }

    void consume(double amount)
    {
        if (canAfford(amount))
        {
            balance -= amount;
        }
        else
        {
            cout << "Insufficient balance for customer: " << name << endl;
        }
    }
};

int checkout(Cart &cart, Customer &customer)
{
    if (cart.countItems() == 0)
    {
        cout << "Cart is empty for customer: " << customer.getName() << "\n";
        return 0;
    }
    double total = cart.getTotal();
    if (customer.canAfford(total))
    {
        customer.consume(total);
        cout << "Purchase successful for customer: " << customer.getName() << "\n";
        cart.printCart();
        cart.purchase();
        return 1;
    }
    cout << "Purchase failed for customer: " << customer.getName() << ". Insufficient balance.\n";
    cout << "required: " << total << ", available: " << customer.getBalance() << "\n";
    return 0;
}

void test_1()
{
    Product cheese("Cheese", 10.0, 0.1, 100);
    Product milk("Milk", 2.0, 0.02, 200);
    Product scratchCard("ScratchCard", 5.0, 0.05, 50);

    Cart cart;
    bool valid_operation = true;
    valid_operation &= cart.addProduct(&cheese, 2);
    valid_operation &= cart.addProduct(&milk, 3);
    valid_operation &= cart.addProduct(&scratchCard, 1);

    if (!valid_operation)
    {
        cout << "Failed to add some products to the cart.\n";
        return;
    }

    Customer customer("John Doe", 50.0);
    checkout(cart, customer);
}

void test_2()
{
    WeightedExpirableProduct cheese("Cheese", 10.0, 0.1, 100, 0.5, "2023-10-15");
    WeightedExpirableProduct milk("Milk", 2.0, 0.02, 200, 0.3, "2023-10-20");
    WeightedExpirableProduct scratchCard("ScratchCard", 5.0, 0.05, 50, 0.1, "2023-10-25");

    Cart cart;
    bool valid_operation = true;
    valid_operation &= cart.addProduct(&cheese, 10);
    valid_operation &= cart.addProduct(&milk, 3);
    valid_operation &= cart.addProduct(&scratchCard, 6);

    if (!valid_operation)
    {
        cout << "Failed to add some products to the cart.\n";
        return;
    }

    Customer customer("John Doe", 1000.0);
    checkout(cart, customer);
}

void test_3()
{
    Product cheese("Cheese", 10.0, 0.1, 100);
    Product milk("Milk", 2.0, 0.02, 200);
    Product scratchCard("ScratchCard", 5.0, 0.05, 50);

    Cart cart;
    bool valid_operation = true;
    valid_operation &= cart.addProduct(&cheese, 2);
    valid_operation &= cart.addProduct(&milk, 3);
    valid_operation &= cart.addProduct(&scratchCard, 1);

    if (!valid_operation)
    {
        cout << "Failed to add some products to the cart.\n";
        return;
    }

    Customer customer("John Doe", 8);
    checkout(cart, customer);
}

int main()
{
    test_1();
    cout << "----------------------------------------------\n";
    test_2();
    cout << "----------------------------------------------\n";
    test_3();
    return 0;
}