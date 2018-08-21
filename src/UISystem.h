#pragma once
#include <string>
#include <string.h>
#include <curses.h>
#include <vector>

namespace mk
{
    #define MIN_COLS 5u
    #define MIN_ROWS 4u

    struct StyledLine
    {
    /*
     *  Scrolling Line
     *  2 types of selections: current and selected
     *  - selected stands for what the user is highlighting
     *  - current is what is currently playing
     *  
     *  current can become the selected item if Apply or Enter
     *  is pressed on what song to play
     */
    public: 
        std::string     data;
        unsigned int    x, y, w;
        bool            isAnimated = false;
        unsigned int    time = 0;

        StyledLine(const std::string& s ,int x, int y, int w = -1)
            :data(s), x(x), y(y), w(w){
                if(w == -1)
                    w = s.size();
            }
        StyledLine(const char* s ,int x, int y, int w= -1)
            :data(s), x(x), y(y), w(w){
                if(w == -1)
                    w = strlen(s);
            }
        void Update (bool sel);
        void Print  (int row, int col);
    private: 
        size_t startOffset = 0;
    };

    class StyledList
    {
    public:
        void Add(std::string element);
        void Remove(std::string element);
        void Print(int row, int col) const;
        inline void Clear() {lines.clear();}

        bool Update(int ch);
        StyledList(int x, int y, int w, int h)
            :x(x), y(y), w(w), h(h) {}

        // Returns currently selected item
        inline StyledLine*          GetSelectedItem         () const {return lines[selectedItem];}
        //  Returns currently selected item index
        inline unsigned long        GetSelectedItemIndex    () const {return selectedItem;}
        // Get an item by index
        inline const StyledLine*    GetItem                 (unsigned int index) const { return lines[index];} 

        // Quick call to lines.size()
        inline size_t GetSize() const { return lines.size();}

        inline unsigned long    GetCurrent              () const {return current;}
        inline StyledLine*      GetCurrentSelectedItem  () const {return lines[current];}
        inline void             ApplySelectedToCurrent  () {current = selectedItem;}

        inline bool Next() {
            if(current+1 < lines.size())
            {
                current++;
                return true;
            }
            return false;
        }

        inline void SetH(int nh) {h = nh;}
        inline void SetW(int nw)
        {
            w = nw;
            for(auto* s : lines)
            {
                s->w = nw;
            }
        }
    private:
        int x, y, w, h;
        size_t displayStart = 0;
        std::vector<StyledLine*> lines;
        // bool isActive = true;
        unsigned int selectedItem = 0;
        unsigned long current = 0;
        void ChangeSelection(int value);

    };

    class UISoundPopup{
        static float            currentVolume;
        static float            timer;
        static float            timeout;
        static bool             shouldAppear;

    public:
        static unsigned int     barSize;
        static float            volumeIncrement;

        static void Print   (unsigned int row, unsigned int col);
        static void Update  (int ch, unsigned long elapsedTime);

        static float GetVolume() {return currentVolume;}
        static void SetVolume(float value) {
            if(value < 0.0f)
                currentVolume = 0.f;
            if(value > 1.0f)
                currentVolume = 1.0f;
        }
    };

    /*
     * Draws progress bar on the last row, spanning all columns
     */
    void ProgressBar(float length, float now, int row, int col, int y);
    // Generic for ProgressBar, making it more flexible
    void ProgressBarGeneric(float length, float now, int row, int col, int x, int y, int size);
    /*
     * Prints string into terminal,
     * if it doesnt fit it can scroll
     * like they do in the news cast
     */
    void PrintOffset(std::string outputStr, int row, int col, unsigned int y);

    /*
     * Prints Vertical separator start from row: starty to endy, in the x collumn
     */
    void PrintVertSeparator(unsigned int row, unsigned int col, unsigned int starty, unsigned int endy, unsigned int x, char c);

    void PrintHoriSeparator(unsigned int row, unsigned int col, unsigned int startx, unsigned int endx, unsigned int y, char c);

    /*
     * Determines whether or not all the
     *  elements can be rendered on the terminal
     */
    bool AssertWindowSize();


}
