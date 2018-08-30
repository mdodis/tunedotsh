#pragma once
#include <string>
#include <string.h>
#include <curses.h>
#include <exception>
#include <vector>

#include <json.hpp>
#include "SoundSystem.h"

using json = nlohmann::json;


namespace mk
{
    #define MIN_COLS 5u
    #define MIN_ROWS 4u

    #define uint unsigned int
    #define ulong unsigned long

    struct UILine
    {
    public: 
        std::string     data;
        unsigned int    x, y, w;
        bool            isAnimated = false;
        unsigned int    time = 0;

        UILine(const std::string& s ,int x, int y, int w = -1, ulong xtra = -1)
            :data(s), x(x), y(y), w(w){
                if(w == -1)
                    w = s.size();
            }
        UILine(const char* s ,int x, int y, int w= -1, ulong xtra = -1)
            :data(s), x(x), y(y), w(w){
                if(w == -1)
                    w = strlen(s);
            }
        void Update (bool sel);
        void Print  (int row, int col);
    private: 
        size_t startOffset = 0;
    };

    struct UITrackLine : UILine
    {
    public: 
        unsigned long   trackNumber;

        UITrackLine(const std::string& s ,int x, int y, int w = -1, ulong xtra = -1)
            : UILine(s, x, y, w), trackNumber(xtra) {}

        UITrackLine(const char* s ,int x, int y, int w= -1, ulong xtra = -1)
            : UILine(s, x, y, w), trackNumber(xtra) {}

    private: 
        size_t startOffset = 0;
    };

    /*! \brief List with selectable items
     *         Moving with arrow keys and pressing enter switches the selection.
     *
     *  Detailed description starts here.
     */
    class UIGenericList
    {
    public:
        UIGenericList(uint x, uint y, uint w, uint h)
            :x(x), y(y), w(w), h(h) {}
 
        /*
         *  List Methods
         */
        virtual void        AddItem     (std::string element, ulong xtra = 0);
        virtual void        RemoveItem  (std::string element);
        virtual void        RemoveItem  (uint index);
        inline virtual void Clear       () {elements.clear();}
        inline size_t       GetSize     () const {return elements.size();}


        virtual void Print(uint row, uint col);
        virtual bool Update(int ch);
        virtual bool Update(int ch, uint row, uint col);

        inline unsigned long      GetSelectedItemIndex    () const {return selectedItem;}
        inline const UITrackLine*  GetItem                 (unsigned long index)
            {return elements[index];}
        inline const UITrackLine* GetSelectedItem          () const 
            {return elements[selectedItem];}

        inline void SetH(int nh) {h = nh;}
        inline void SetW(unsigned long nw)
        {
            w = nw;
            for(auto* s : elements)
            {
                s->w = nw;
            }
        }
        inline void SetX(int nx) {x = nx;}
        inline void SetY(int ny) {y = ny;}

        inline unsigned long GetX () const {return x;}
        inline unsigned long GetY () const {return y;}
        inline unsigned long GetW () const {return w;}
        inline unsigned long GetH () const {return h;}

        inline bool GetFocus() const { return isFocus;}
        inline void SetFocus(bool value) {isFocus = value;}

        inline const std::vector<UITrackLine*>& GetElVec() const { return elements; }

    protected:
        uint x, y, w, h;
        size_t displayStart = 0;
        std::vector<UITrackLine*> elements;

        // bool isActive = true;
        unsigned long selectedItem = 0;
        virtual void ChangeSelection(int value);

        bool isFocus = false;
    };

    class UIReorderList : public UIGenericList
    {
    public:
        UIReorderList(uint x, uint y, uint w, uint h) : UIGenericList(x,y,w,h)
        {}

        void Print(uint row, uint col) override;
        bool Update(int ch) override;
        bool Update(int ch, uint row, uint col) override;


        inline ulong            GetCurrentItemIndex     () const {return currentItem;}
        const UITrackLine* const GetCurrentItem          () const {return elements[currentItem];}

        inline void             SetCurrentItemIndex     (ulong value) {currentItem = value; }
        inline void             SetCurrentToSelected    () {currentItem = selectedItem;}
        void                    SetCurrentItem          (std::string name);

        //  Adds val to current, returns false if it exceeds the bounds of the elements list
        inline bool             ChangeCurrent           (long val)
        {
            long change = (long)currentItem + val;
            if (change >= elements.size() || change < 0)
                return false;
            currentItem = change;
            return true;
        }

        inline void SetPlaylist(Playlist* p)
        {
            size_t index = 0;
            this->Clear();
            for(SongInfo* s : p->songs)
            {
                UITrackLine* l = new UITrackLine(s->fileName, x, y + elements.size(), w, index);
                elements.push_back(l);
                index++;
            }
        }
    private:
        unsigned long currentItem = 0;

        void PushSelected(int m);
        inline bool SwapItem(size_t a, size_t b)
        {
            if (a < elements.size() && b < elements.size())
            {
                std::swap(elements[a], elements[b]);
                return true;
            }
            return false;
        }

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
    void PrintVertSeparator(unsigned int row, unsigned int col, unsigned int starty, unsigned int endy, unsigned int x, wint_t c);

    void PrintHoriSeparator(unsigned int row, unsigned int col, unsigned int startx, unsigned int endx, unsigned int y, char c);
    void PrintHoriSeparator(unsigned int row, unsigned int col, unsigned int startx, unsigned int endx, unsigned int y, wint_t c);

    /*
     * Determines whether or not all the
     *  elements can be rendered on the terminal
     */
    bool AssertWindowSize();


}
