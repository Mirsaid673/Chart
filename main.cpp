#include "ChartApp.h"

int main() {
    std::unique_ptr<AppBase> app = std::make_unique<ChartApp>();
    return app->run();
}