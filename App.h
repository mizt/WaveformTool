#import <Cocoa/Cocoa.h>
#import "AudioIn.h"

class App {
	
    private :
    
        Boolean isLock;
        AITimerHandle timer;
        AudioIn *audio;
    
        vector<double> dots;
        vector<AIArtHandle> buffer;
    
        dispatch_source_t enterFrame;
    
    public :
    
        AITimerHandle *getTimer() { return &timer; }
    
        App() {
            
            for(int i=0; i<64; i++) this->dots.push_back(0);
            
            this->isLock = true;
            
            this->audio = new AudioIn();
            
            this->enterFrame = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER,0,0,dispatch_queue_create("WavrformTool",0));
            dispatch_source_set_timer(this->enterFrame,dispatch_time(0,0),(1.0/24)*1000000000,0);
            dispatch_source_set_event_handler(this->enterFrame,^(){
                dispatch_async(dispatch_get_main_queue(),^{
                    if(this->isLock==false) {
                        if([NSApplication sharedApplication].active==YES) {
                            CGEventRef e = CGEventCreate(NULL);
                            CGEventRef m = CGEventCreateMouseEvent(NULL,kCGEventMouseMoved,CGEventGetLocation(e),kCGMouseButtonLeft);
                            CGEventPost(kCGHIDEventTap,m);
                            CFRelease(e);
                            CFRelease(m);
                        }
                    }
                });
            });
            dispatch_resume(this->enterFrame);
        }
    
        ~App() {
            if(this->enterFrame){
                dispatch_source_cancel(this->enterFrame);
                this->enterFrame = nullptr;
            }
            delete this->audio;
            dots.clear();
        }
    
        void drawPoint(AIPathSegment *segments,int index,double x, double y) {
            segments[index].p.h   = x;
            segments[index].p.v   = y;
            segments[index].in.h  = x;
            segments[index].in.v  = y;
            segments[index].out.h = x;
            segments[index].out.v = y;
            segments[index].corner = false;
        }
    
        void draw(AITimerMessage *message=NULL) {
            
            if(this->isLock) return;
            
            ASErr error = kNoErr;
            
            if(error==kNoErr) {
                
                ai::ArtboardList artboardList;
                error = sAIArtboard->GetArtboardList(artboardList);
                if(error!=kNoErr) return;
                
                ai::ArtboardID aid;
                error = artboardList.GetActive(aid);
                if(error!=kNoErr) return;
                
                ai::ArtboardProperties prop = artboardList.GetArtboardProperties(aid);
                
                AIRealRect rect = {0,0,0,0};
                error = prop.GetPosition(rect);
                if(error!=kNoErr) return;
                
                double width  = rect.right-rect.left;
                double height = rect.top-rect.bottom;
                
                int size = (int)buffer.size();
                int len = size;
                
                while(len--) { sAIArt->DisposeArt(buffer[len]); }
                buffer.clear();
                
                for(int i=0;i<dots.size();i++) {
                    double wav = (1.0+(this->audio->buffer[i*4]))*0.5;
                    if(wav<=0.0) wav = 0.0;
                    if(wav>=1.0) wav = 1.0;
                    dots[i] = height*wav;
                }
                buffer.push_back(NULL);
                int tmp = ((int)buffer.size())-1;
                error = sAIArt->NewArt(kPathArt,kPlaceAboveAll,NULL,&(buffer[tmp]));
                if(error==kNoErr) {
                    sAIArt->SetArtUserAttr(buffer[tmp],kArtSelected,kArtSelected);
                    AIPathSegment segments[dots.size()];
                    for(int k=0;k<dots.size();k++) drawPoint(segments,k,k*width/(dots.size()-1),-dots[k]);
                    sAIPath->SetPathSegments(buffer[tmp],0,dots.size(),segments);
                    sAIPath->SetPathClosed(buffer[tmp],true);
                }
            }
        }
    
        void lock() {
            this->isLock = true;
            sAITimer->SetTimerActive(this->timer,!this->isLock);
            this->audio->stop();
        }
        
        void unlock() {
            isLock = false;
            sAITimer->SetTimerActive(this->timer,!this->isLock);
            this->audio->start();
        }
};