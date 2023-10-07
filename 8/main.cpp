#include <iostream>
#include <vector>
#include <algorithm>

struct Segment{
    int left;
    int right;
    int ind;
};

int compare(Segment& a, Segment& b){
    if(b.right == a.right)
        return b.left > a.left;
    return b.right > a.right;
}

int compareIND(Segment& a, Segment& b){
    return b.ind > a.ind;
}

std::vector<Segment> greedyfind(std::vector<Segment> &Segments, int M){
    std::vector<Segment> selected;
    Segment begin;
    begin.left=0;
    begin.right=0;
    begin.ind=-1;
    selected.push_back(begin);
    std::vector<Segment> sorted;
    sorted = Segments;
    std::sort(sorted.begin(),sorted.end(), compare);
    std::vector<Segment>::reverse_iterator it;
    while(1) {
        int flag=0;
        for (it = sorted.rbegin(); it != sorted.rend(); ++it) {
            Segment &segment = *it;
            if (segment.left <= selected.back().right && segment.right > selected.back().right) {
                selected.push_back(segment);
                flag = 1;
                break;
            }
        }
        if(selected.back().right >= M)
            break;
        if(!flag) {
            selected.clear();
            break;
        }
    }
    std::sort(selected.begin(),selected.end(), compareIND);
    return selected;
}

int main(){
    int n;
    std::cin >> n;
    std::vector<Segment> Segments(n);
    for(int i=0;i<n;i++){
        std::cin>>Segments[i].left>>Segments[i].right;
        Segments[i].ind = i;
    }
    int M;
    std::cin>>M;
    std::vector<Segment> selected;
    selected = greedyfind(Segments, M);

    if(selected.size()<2){
        std::cout << 0;
    }else{
        std::vector<Segment>::iterator it;
        std::cout << selected.size() -1 << "\n" ;
        for (it =  std::next(selected.begin()); it != selected.end(); ++it) {
            const Segment &segment = *it;
            std::cout << segment.left << " " << segment.right << "\n";
        }
    }
    return 0;

}