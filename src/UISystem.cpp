#include "UISystem.h"
#include <curses.h>
#include <iostream>
#include <string>
namespace mk{

    void StyledLine::Print(int row, int col)
    {
        std::string outputData = data;
        if(data.length() > (unsigned long)w)
        {

            outputData = outputData.substr(startOffset, w);
        }
        else
            outputData = data;
        mvprintw(y, x, outputData.c_str());
    }

    void StyledLine::Update(bool sel)
    {
        if(data.length() > (unsigned long)w)
        {
            if(!isAnimated)
            {
                startOffset = 0;
                isAnimated = true;
            }
            if(isAnimated)
                startOffset++;
            if(startOffset + w > data.length())
            {
                isAnimated = false;
                startOffset = 0;
            }

            if(!sel)
            {
                isAnimated = false;
                startOffset = 0;
            }
        }
    }

    void StyledList::ChangeSelection(int value)
    {
        if((int)selectedItem + value >= 0 && (int)(selectedItem + value) < (int)lines.size())
        {
            selectedItem += value;
        }
        if(selectedItem >= displayStart + h || selectedItem < displayStart)
        {
            displayStart += value;
        }
    }

    bool StyledList::Update(int ch)
    {
        switch(ch)
        {
            case KEY_UP:
                ChangeSelection(-1);
                break;
            case KEY_DOWN:
                ChangeSelection(1);
                break;
            case '\n':
                return true;
                break;
            default:
                break;
        }
        // lines[selectedItem]->Update();
        
        for(size_t i = 0; i < lines.size(); i++)
        {
            if(i == selectedItem)
                lines[i]->Update(true);
            else
                lines[i]->Update(false);
        }
        
        return false;

    }
    void StyledList::Add(std::string element)
    {
        StyledLine* l = new StyledLine(element, x,y + lines.size(),w);
        lines.push_back(l);
    }

    void StyledList::Remove(std::string element)
    {
        std::vector<StyledLine*>::iterator i;
        
        for( i = lines.begin(); i != lines.end(); i++)
        {
            if((**i).data == element)
            {
                delete *i;
                lines.erase(i);
            }
        }
        
    }

    void StyledList::Print(int row, int col) const
    {
        unsigned int count = 0;
        size_t tmp = lines.size() > (size_t)h ? h : lines.size();
        if(displayStart + h > lines.size())
        {
            tmp = lines.size();
        }
        else {tmp = displayStart + h;}
        for(unsigned long i = displayStart; i < tmp; ++i)
        {
            lines[i]->x = x ;
            lines[i]->y = y + count;
            if(i == selectedItem)
                attron(A_STANDOUT);
            else if(i == current)
                attron(A_UNDERLINE);
            lines[i]->Print(row, col);
            attroff(A_STANDOUT);
            attroff(A_UNDERLINE);
            count++;
        }
    }
    /*
     * returns the amount of cols that now should span
     * with a total size of 'col' columns and the limits
     * between 0 and length
     */
    unsigned int CountBars(float now, float length, int col)
    {
        int c= 0;
        float colSize = length / (float)col;
        while(now > colSize * c)
        {
            c++;
        }
        return c;
    }


    void ProgressBar(float length, float now, int row, int col, int y)
    {

        if(length == 0.0f || length < now)
            return;
        int curCol = CountBars(now, length, col);
        if(curCol - 1 <= 0) return;
        std::string p(curCol - 1, '=');
        p += '>';
        // std::cout << curCol << ' ' << p << '\n';
        mvprintw(y,0 , p.c_str());
    }

    void ProgressBarGeneric(float length, float now, int row, int col, int x, int y, int size)
    {
        if(size < 3)
            return;
        if(length <= 0.0f || length < now)
            return;

        if(size + 2 > col) return;

        int curCol = CountBars(now, length, size);
        std::string p;
        if(curCol -1 > 0)
            p.insert(0,curCol - 1, '=');
        if(size - curCol - 1 > 0)
            p.insert(p.end(), size - curCol , ' ');
        p += ']';
        p.insert(0, "[");
        mvprintw(y, x, p.c_str());
    }



    void PrintOffset(std::string outputStr, int row, int col, unsigned int y)
    {
        if (outputStr.length() > (unsigned long)col)
            return;
        mvprintw(y,0,outputStr.c_str());
    }


    bool AssertWindowSize()
    {
        unsigned int row, col;
        getmaxyx(stdscr, row, col);
        if ( row < MIN_ROWS || col < MIN_COLS) return false;
        else return true;
    }

    void PrintVertSeparator(unsigned int row, unsigned int col, unsigned int starty, unsigned int endy, unsigned int x, char c)
    {
        if (starty < 0 || endy > row || endy - starty > row || x > col)
            return;
        for(unsigned int i = starty; i <= endy; ++i)
        {
            mvprintw(i, x, "%c", c);
        }

    }

    void PrintHoriSeparator(unsigned int row, unsigned int col, unsigned int startx, unsigned int endx, unsigned int y, char c)
    {
        if (startx < 0 || endx > col || endx - startx > col || y > row)
            return;
        for (int i = startx; i <= endx; ++i)
        {
            mvprintw(y, i, "%c", c);
        }

    }




    /*  UISoundPopup
     */

    float           UISoundPopup::currentVolume = 1.0f;
    float           UISoundPopup::timer = 0.0f;
    float           UISoundPopup::timeout = 1.5f;
    bool            UISoundPopup::shouldAppear = false;

    unsigned int    UISoundPopup::barSize = 20;
    float           UISoundPopup::volumeIncrement = 0.1f;
    void UISoundPopup::Print(unsigned int row, unsigned int col)
    {
        // if (timer > 0.000001f && timer < timeout)
        if (shouldAppear)
            ProgressBarGeneric(1.0f, currentVolume, row, col, (col-barSize)/2, 0, barSize + 1);
    }
    void UISoundPopup::Update(int ch, unsigned long elapsedTime)
    {
        if(ch == 'k')
        {
            shouldAppear = true;
            timer = 0.f;
            if(currentVolume > 0.f)
                currentVolume -= volumeIncrement;
        }
        else if (ch == 'j')
        {
            shouldAppear = true;
            timer = 0.f;
            if(currentVolume < 1.f)
                currentVolume += volumeIncrement;   
        }

        if(shouldAppear)
        {
            timer += 0.3f;
        }

        if(timer >= timeout)
        {
            shouldAppear = false;
        }
    }
    
}