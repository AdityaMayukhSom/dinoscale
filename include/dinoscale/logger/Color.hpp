#include <string>
/**
 * @brief An class containing color codes of ANSII color values.
 * @note `FG_X` represents foreground color  as `X` whereas
 * `BG_X` represents background color as `X`.
 */
class Color {
    /**
     * @brief Converts passed color value into printable string.
     */
    static const std::string getColorString(int colorCode) {
        return "\033[" + std::to_string(colorCode) + "m";
    }

   public:
    /* For consistency, can reset both background and foreground color. */
    inline static const std::string FG_RESET = getColorString(0);

    inline static const std::string FG_BLACK = getColorString(30);
    inline static const std::string FG_RED = getColorString(31);
    inline static const std::string FG_GREEN = getColorString(32);
    inline static const std::string FG_YELLOW = getColorString(33);
    inline static const std::string FG_BLUE = getColorString(34);
    inline static const std::string FG_MAGENTA = getColorString(35);
    inline static const std::string FG_CYAN = getColorString(36);
    inline static const std::string FG_WHITE = getColorString(37);
    inline static const std::string FG_DEFAULT = getColorString(39);

    inline static const std::string BG_BLACK = getColorString(40);
    inline static const std::string BG_RED = getColorString(41);
    inline static const std::string BG_GREEN = getColorString(42);
    inline static const std::string BG_YELLOW = getColorString(43);
    inline static const std::string BG_BLUE = getColorString(44);
    inline static const std::string BG_MAGENTA = getColorString(45);
    inline static const std::string BG_CYAN = getColorString(46);
    inline static const std::string BG_WHITE = getColorString(47);
    inline static const std::string BG_DEFAULT = getColorString(49);
};